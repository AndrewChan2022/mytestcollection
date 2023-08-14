//
//  JobSystem.hpp
//
//
//  Created by kai chen on 3/24/23.
//

#pragma once

#include <cstdlib>
#include <string>
#include <functional>
#include <chrono>
#include <iostream>


struct TestTimer {
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
 
    TestTimer(const std::string& name) : name(name), start(std::chrono::high_resolution_clock::now()) {}
    ~TestTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << name << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds" << std::endl;
    }
};
 
// A Dispatched job will receive this as function argument:
struct JobDispatchArgs {
    uint32_t jobIndex;
    uint32_t groupIndex;
};

namespace JobSystem {

    // Create the internal resources such as worker threads, etc. Call it once when initializing the application.
    void Initialize();
 
    // Add a job to execute asynchronously. Any idle thread will execute this job.
    void Execute(const std::function<void()>& job);
 
    // Divide a job onto multiple jobs and execute in parallel.
    //  jobCount    : how many jobs to generate for this task.
    //  groupSize   : how many jobs to execute per thread. Jobs inside a group execute serially. It might be worth to increase for small jobs
    //  func        : receives a JobDispatchArgs as parameter
    //void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs&)>& job);
    void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs&)>& job);
 
    // Check if any threads are working currently or not
    bool IsBusy();
 
    // Wait until all threads become idle
    void Wait();
}
