//
// Copyright 2023-2024 Chen Kai
//
// This program is free software: you can redistribute it and/or modify it under the terms 
// of the GNU General Public License as published by the Free Software Foundation, either 
// version 3 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program. 
// If not, see <https://www.gnu.org/licenses/>.
//

//
// Implement 
//

#include "DispatchQueue.h"
#include <future>

namespace feynman {

// https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/dispatch.cpp


// #define DEBUG_DISPATCH
#ifdef DEBUG_DISPATCH
// #define DISPATCH_QUEUE_PRINTF(f_, ...) printf((f_), __VA_ARGS__)
// #define DISPATCH_QUEUE_PRINTF(...) printf(__VA_ARGS__)
#define DISPATCH_QUEUE_PRINTF(f_, ...) printf((f_), ##__VA_ARGS__)
#else
    #define DISPATCH_QUEUE_PRINTF(f_, ...) 
#endif

DispatchQueue::Ptr DispatchQueue::getBackgroundQueue() {
    static DispatchQueue::Ptr s_defaultBackgroundQ = nullptr;
    if(s_defaultBackgroundQ == nullptr) {
        s_defaultBackgroundQ = std::make_shared<DispatchQueue>();
    }
    return s_defaultBackgroundQ;
}

DispatchQueue::DispatchQueue() : name_{"default_q"}, threadsCount_(1) {
    // DISPATCH_QUEUE_PRINTF("Creating dispatch queue: %s\n", name_.c_str());
    // DISPATCH_QUEUE_PRINTF("Dispatch threads: %d\n", threadsCount_);

    for(size_t i = 0; i < threadsCount_; i++) {
        std::thread t = std::thread(&DispatchQueue::dispatch_thread_handler, this);
        threads_.push_back(std::move(t));
    }
    idleCount_ = 1;
}


DispatchQueue::DispatchQueue(std::string name, size_t thread_cnt) 
: name_{std::move(name)}, threadsCount_((int)thread_cnt) {
    
    // DISPATCH_QUEUE_PRINTF("Creating dispatch queue: %s\n", name_.c_str());
    // DISPATCH_QUEUE_PRINTF("Dispatch threads: %d\n", threadsCount_);

    for(size_t i = 0; i < threadsCount_; i++) {
        std::thread t = std::thread(&DispatchQueue::dispatch_thread_handler, this);
        threads_.push_back(std::move(t));
    }
    idleCount_ = (int)thread_cnt;
}

DispatchQueue::~DispatchQueue() {

    DISPATCH_QUEUE_PRINTF("Destructor: Destroying dispatch threads %s ... \n", name_.c_str());
    
    // Signal to dispatch threads that it's time to wrap up
    std::unique_lock<std::mutex> lock(mutex_);
    quit_ = true;
    lock.unlock();
    cv_.notify_all();

    // Wait for threads to finish before we exit
    for(size_t i = 0; i < threads_.size(); i++) {
        if(threads_[i].joinable()) {
            DISPATCH_QUEUE_PRINTF("Destructor: Joining thread %zu until completion\n", i);
            threads_[i].join(); // join or detach
            DISPATCH_QUEUE_PRINTF("Destructor: Joining thread %zu completion\n", i);
        }
    }
    DISPATCH_QUEUE_PRINTF("Destructor: Destroying dispatch threads %s done\n", name_.c_str());
}

// MARK: - dispatch
void DispatchQueue::dispatch_sync(std::function<void()> block) {
    
    std::unique_lock<std::mutex> lock1(sync_mutex_);
    bool finish = false;
    
    this->dispatch([this, block, &finish] {
        std::unique_lock<std::mutex> lock2(sync_mutex_);
        block();
        // produce semphor when finish
        finish = true;
        sync_cv_.notify_all();
    });
    
    // wait to consume semphor
    sync_cv_.wait(lock1, [this, &finish]{return finish;}); // whether to capture this is not matter
    //cout << "wait finish" << endl;
}

void DispatchQueue::dispatch_async(std::function<void()> block) {
    this->dispatch(block);
}

void DispatchQueue::dispatch_asyncAfter(float delaySeconds, std::function<void()> block) {

    // TODO: need optimize dispatch_asyncAfter
    // another implement: get a clean thread from pool, then put a sleep at beginging of the code
    // sleep in new dispatch queue, then call dispatch_async
    int delayMs = delaySeconds * 1000;
    std::shared_ptr<DispatchQueue> tempQ = std::make_shared<DispatchQueue>("tempQ", 1);
    tempQ->dispatch_async([this, block, delayMs, tempQ]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        this->dispatch([block, tempQ]() {
            block();
            
            // capture it to avoid destroy, will destroy on return
            // if capture in tempQ->dispatch_async, then destroy on return, which wait tempQ->dispatch_async to end
            auto a = tempQ;
        });
    });
    
    tempQ = nullptr;
}

// MARK: - private

void DispatchQueue::dispatch(const fp_t& op) {

    std::unique_lock<std::mutex> lock(mutex_);
    q_.push(op);
    qsize_++;

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lock.unlock();
    cv_.notify_all();
}

void DispatchQueue::dispatch(fp_t&& op) {

    std::unique_lock<std::mutex> lock(mutex_);
    q_.push(std::move(op));
    qsize_++;

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lock.unlock();
    cv_.notify_all();
}

void DispatchQueue::dispatch_thread_handler(void) {

    std::unique_lock<std::mutex> lock(mutex_);

    do {
        //Wait until we have data or a quit signal
        cv_.wait(lock, [this]{
            return (q_.size() || quit_);
        });

        //after wait, we own the lock
        if(!quit_ && q_.size()) {
            auto op = std::move(q_.front());
            q_.pop();
            qsize_--;
            idleCount_--;
            //printf("doing...\n");

            //unlock now that we're done messing with the queue
            lock.unlock();

            op();

            lock.lock();
            idleCount_++;
            cv_.notify_all();
        }
    } while (!quit_);

    DISPATCH_QUEUE_PRINTF("thread: end\n");
}

void DispatchQueue::finish() {
    
    std::unique_lock<std::mutex> lock(mutex_);
    
    //printf("waiting...\n");
    //
    cv_.wait(lock, [this] {
        return (idleCount_ >= threadsCount_ && qsize_ == 0) || quit_;
    });
    
    DISPATCH_QUEUE_PRINTF("wait done\n");
}

bool DispatchQueue::isFinish() {
    return (idleCount_ >= threadsCount_ && qsize_ == 0) || quit_;
}


} // namespace feynman

