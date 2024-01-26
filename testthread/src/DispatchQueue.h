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


#pragma once


#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <condition_variable>


namespace feynman {

//https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/dispatch.cpp


/// usage:
///
/// DispatchQueue::Ptr q = DispatchQueue::create();
/// DispatchQueue::Ptr q = DispatchQueue::create(name, threadCount);
/// q->dispatch_async([](){});
/// q->dispatch_sync([](){});

class DispatchQueue {
    typedef std::function<void(void)> fp_t;

public:
    using Ptr = std::shared_ptr<DispatchQueue>;
    
public:
    // --------------------- create
    static Ptr getBackgroundQueue();
    static Ptr create() { return std::make_shared<DispatchQueue>(); }
    static Ptr create(std::string name, size_t thread_cnt = 1) { return std::make_shared<DispatchQueue>(name, thread_cnt);}
    virtual ~DispatchQueue();
    DispatchQueue();
    DispatchQueue(std::string name, size_t thread_cnt = 1);
    
    DispatchQueue(const DispatchQueue& rhs) = delete;
    DispatchQueue& operator=(const DispatchQueue& rhs) = delete;
    DispatchQueue(DispatchQueue&& rhs) = delete;
    DispatchQueue& operator=(DispatchQueue&& rhs) = delete;
    
    // --------------------- dispatch
    void dispatch_sync(std::function<void()> block);
    void dispatch_async(std::function<void()> block);
    void dispatch_asyncAfter(float delaySeconds, std::function<void()> block);
    
    // --------------------- wait all finish
    void finish();
    bool isFinish();

private:
    

    void dispatch_thread_handler(void);
    void dispatch(const fp_t& op); // dispatch and copy
    void dispatch(fp_t&& op); // dispatch and move

private:
    std::string name_;
    std::mutex mutex_;
    std::vector<std::thread> threads_;
    std::queue<fp_t> q_;
    std::condition_variable cv_;
    bool quit_ = false;
    
    std::condition_variable sync_cv_;
    std::mutex sync_mutex_;

    int idleCount_{1};
    int qsize_{0};
    int threadsCount_{1};
};

} // namespace feynman