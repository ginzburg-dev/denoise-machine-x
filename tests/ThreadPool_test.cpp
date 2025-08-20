#include <gtest/gtest.h>

#include "AssertLogContains.hpp"
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/ThreadPool.hpp>

#include <chrono>
#include <iostream>
#include <future>
#include <mutex>
#include <string>
#include <string_view>

using namespace dmxdenoiser;

std::string logFilePath = "../tests/test_files/dmxdenoiser_threadPool_test.log";
bool success = std::filesystem::remove(logFilePath); // Remove log file
std::mutex tpMutex;


TEST(ThreadPool, Init){
    DMX_LOG_INIT(LogLevel::Debug, &std::clog, logFilePath);
    int threads = 4;
    ASSERT_NO_THROW(ThreadPool threadPool(threads));
    assertLogContains(logFilePath, "DEBUG", "ThreadPool", "created with");
}

TEST(ThreadPool, RunSimpleTasksMaxThreads){
    int threads = 0;
    ThreadPool threadPool(threads);

    static int taskN = 0;

    auto func = [](){
        int currentTask{ taskN++ };
        DMX_LOG_DEBUG("ThreadPool", "Task ", currentTask, " is starting on ", std::this_thread::get_id(), " thread."); // Thread safe 
        
        // Processing task
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        DMX_LOG_DEBUG("ThreadPool", "Task ", currentTask, " sucessfully completed."); // Thread safe
    };

    std::vector<std::future<void>> futures;
    for(int i = 0; i < 4; ++i)
    {
        futures.push_back(threadPool.enqueue(func));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }


    for (auto& f : futures)
        f.wait();

    //assertLogContains(logFilePath, "DEBUG", "ThreadPool", "sucessfully completed");
}
