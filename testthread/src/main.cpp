#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <utility>
#include "Async.h"

// #include "Utils/Threads/Async.h"

using namespace std::chrono_literals;

void f1(std::string name, int n) {
    for (int i = 0; i < n; ++i) {
        std::cout << "Thread " + name + " executing " + std::to_string(i) + "\n";
        // ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {

    // {
    //     std::thread t1(f1, "name1", 2);
    //     std::thread t2(f1, "name2", 2);
    //     t1.join();
    //     t2.join();
    // }

    // {
    //     std::thread t1(f1, "name1", 2);
    //     std::thread t2;
    //     // init to not thread
    //     t2 = std::thread(f1, "name2", 2);
    //     t1.join();
    //     t2.join();
    // }

    // {
    //     std::thread t1(f1, "name1", 2);
    //     // move to define
    //     std::thread t2(std::move(t1));  // t2 is now running. t1 is no longer a thread
    //     t2.join();
    // }

    // {
    //     std::thread t1(f1, "name1", 2);
    //     std::thread t2(f1, "name2", 2);
    //     // init to thread, lead to old thread destructor and crash, because it is still joinable
    //     // t2 = std::thread(f1, "name3", 2);  // can init, but crash
    //     t1.join();
    //     // t2.join();
    // }

    std::cout << "before async\n";
    feynman::Utils::asyncWithNewThread([](){
        std::cout << "in async begin\n";
        //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        float a = 0;
        for (size_t i = 0; i < 1000000; i++) {
            a += i * i;
        }
        std::cout << a << std::endl;
        std::cout << "in async end\n";
    });
    std::cout << "after async\n";
    std::this_thread::sleep_for(1500ms);


    std::cout << "before async\n";
    feynman::Utils::async([](){
        std::cout << "in async begin\n";
        //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        float a = 0;
        for (size_t i = 0; i < 1000000; i++) {
            a += i * i;
        }
        std::cout << a << std::endl;
        std::cout << "in async end\n";
    });
    std::cout << "after async\n";
    std::this_thread::sleep_for(1500ms);


    std::cout << "end main\n";
}
