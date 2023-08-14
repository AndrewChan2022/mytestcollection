//
//  mylib.cpp
//
//
//  Created by kai chen on 3/24/23.
//

#include "JobSystem.hpp"
 
#include <iostream>
#include <chrono>
#include <string>
#include <thread>


struct TestData {
    float m[1600];
    void Compute(uint32_t value);
};

void TestData::Compute(uint32_t value) {
    for (int i = 0; i < 1600; ++i) {
        m[i] += float(value + i);
    }
}

/// @brief test
static uint32_t s_dataCount = 1000000;
TestData* g_dataSet = nullptr;

 
 
static void Spin(float milliseconds) {
    milliseconds /= 1000.0f;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    double ms = 0;
    while (ms < milliseconds) {
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        ms = time_span.count();
    }
}
 
int main() {

    JobSystem::Initialize();

    int jobCount = 6;
    uint32_t dataCount = 1000000;
    
 #if 0
    // Serial test
    {
        auto t = TestTimer("Serial test: ");
        for(int i = 0; i < jobCount; i++) {
            Spin(100);
        }
        
        // Spin(100);
        // Spin(100);
        // Spin(100);
        // Spin(100);
        // Spin(100);
        // Spin(100);
    }
 
    // Execute test
    {
        auto t = TestTimer("Execute() test: ");
        for(int i = 0; i < jobCount; i++) {
            JobSystem::Execute([] { Spin(100); });
        }

        // JobSystem::Execute([] { Spin(100); });
        // JobSystem::Execute([] { Spin(100); });
        // JobSystem::Execute([] { Spin(100); });
        // JobSystem::Execute([] { Spin(100); });
        // JobSystem::Execute([] { Spin(100); });
        // JobSystem::Execute([] { Spin(100); });
        JobSystem::Wait();
    }

    
#endif
    
    {
        extern TestData* g_dataSet;
        if (g_dataSet == nullptr) { g_dataSet = new TestData[dataCount]; }
        TestData* dataSet = g_dataSet;
        std::thread t1([=] {
            auto timer = TestTimer("thread1 test: ");
            for (uint32_t i = 0; i < dataCount; ++i)
            {
                dataSet[i].Compute(i);
            }
        });
        std::thread t2([=] {
            auto timer = TestTimer("thread2 test: ");
            for (uint32_t i = 0; i < dataCount; ++i)
            {
                dataSet[i].Compute(i);
            }
        });
        t1.detach();
        t2.detach();
    }

    {
        //TestData* dataSet = new TestData[dataCount];
        extern TestData* g_dataSet;
        if (g_dataSet == nullptr) { g_dataSet = new TestData[dataCount]; }
        TestData* dataSet = g_dataSet;
        {
            auto t = TestTimer("loop test: ");
            for (uint32_t i = 0; i < dataCount; ++i)
            {
                dataSet[i].Compute(i);
            }
        }
        //delete[] dataSet;
    }

    // Loop test:
    {
        //TestData* dataSet = new TestData[dataCount];
        extern TestData* g_dataSet;
        if (g_dataSet == nullptr) { g_dataSet = new TestData[dataCount]; }
        TestData* dataSet = g_dataSet;
        JobSystem::Execute([=]
        {
            auto t = TestTimer("Execute test: ");

            for (uint32_t i = 0; i < dataCount; ++i)
            {
                dataSet[i].Compute(i);
            }
        });

        JobSystem::Wait();
        //delete[] dataSet;
        
    }

    {
        //TestData* dataSet = new TestData[dataCount];
        extern TestData* g_dataSet;
        if (g_dataSet == nullptr) { g_dataSet = new TestData[dataCount]; }
        TestData* dataSet = g_dataSet;
        JobSystem::Execute([=]
            {
                auto t = TestTimer("Execute test: ");

        for (uint32_t i = 0; i < dataCount; ++i)
        {
            dataSet[i].Compute(i);
        }
            });
        //delete[] dataSet;
        JobSystem::Wait();
    }


    // Dispatch test:
    {
        //TestData* dataSet = new TestData[dataCount];
        extern TestData* g_dataSet;
        if (g_dataSet == nullptr) { g_dataSet = new TestData[dataCount]; }
        TestData* dataSet = g_dataSet;
        {
            auto t = TestTimer("Dispatch() test: ");

            const uint32_t groupSize = 1000000/32;
            JobSystem::Dispatch(dataCount, groupSize, [&dataSet](JobDispatchArgs& args) {
                dataSet[args.jobIndex].Compute(args.jobIndex);
            });
            JobSystem::Wait();
        }
        //delete[] dataSet;
        std::cout << "end" << std::endl;
    }
 
    return 0;
}


// summay:
// worker thread = 4,  fast as main
// worker thread = cpu, slow than main
// worker thread parallel, memory not so fast
