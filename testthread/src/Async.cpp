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

#include "Async.h"
#include "DispatchQueue.h"
#include <unordered_set>

namespace feynman::Utils {

struct AsyncData {
    DispatchQueue dispatchQueue;
    std::unordered_set<void*> threadsSet;
    int32_t isSingletonStillLiving{0xff};

    ~AsyncData() {
        isSingletonStillLiving = 0;
    }
};

static AsyncData& _getAsyncData() {
    static AsyncData s_AsyncData;
    return s_AsyncData;
}

struct PoolData {
    DispatchQueue dispatchQueue{"async pool", 8};
    int32_t isSingletonStillLiving{0xff};

    ~PoolData() {
        isSingletonStillLiving = 0;
    }
};

static PoolData& _getPoolData() {
    static PoolData s_PoolData;
    return s_PoolData;
}

void asyncWithNewThread(const std::function<void(void)>& op) {

    // get static data
    auto& data = _getAsyncData();
    DispatchQueue* pQueue = &data.dispatchQueue;
    std::unordered_set<void*>* pPool = &data.threadsSet;
    int32_t* pIsAppStillLive = &data.isSingletonStillLiving;

    // create inactive thread
    std::thread* pThread = new std::thread();

    // init with active thread, which will do the heavy job, afther finish job, then delete
    *pThread = std::thread([=]() {

        // heavy job, will 
        op();

        // light weight job to delete this thread
        if (pIsAppStillLive) {
            pQueue->dispatch_async([=](){
                // printf("delete thread begin\n");
                pPool->erase(pThread);
                if (pThread->joinable()) {
                    pThread->join();
                }
                delete pThread;
                // printf("delete thread done\n");
            });
        }
        // printf("done async\n");
    });

    // detach
    // pThread->detach();
}

void async(const std::function<void(void)>& op) {
    PoolData& data = _getPoolData();
    data.dispatchQueue.dispatch_async([=]() {
        op();
    });
}

void asyncWithBackgroundQueue(const std::function<void(void)>& op) {
    DispatchQueue::getBackgroundQueue()->dispatch_async([=]() {
        op();
    });
}

} // namespace feynman
