#include <gtest/gtest.h>

#include "AssertLogContains.hpp"
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/ThreadPool.hpp>

#include <chrono>
#include <iostream>
#include <limits>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

using namespace dmxdenoiser;

class ThreadPoolTest : public ::testing::Test {
protected:
    std::string logFilePath = "../tests/test_files/dmxdenoiser_threadPool_test.log";
    std::mutex tpMutex;
    
    void SetUp() override {
        bool success = std::filesystem::remove(logFilePath); // Remove log file
        DMX_LOG_INIT(LogLevel::Trace, &std::clog, logFilePath);
    }
};

TEST_F(ThreadPoolTest, Init){
    const int threads = 4;
    std::unique_ptr<ThreadPool> tp;

    ASSERT_NO_THROW(
        tp = std::make_unique<ThreadPool>(threads);
    );
    
    assertLogContains(logFilePath, "INFO", "ThreadPool", "created with");
    assertLogDoesNotContain(logFilePath, "ERROR");
}

TEST_F(ThreadPoolTest, InitWith0ValueHenseMaxThreads){
    const unsigned int threads = 0;
    std::unique_ptr<ThreadPool> tp;

    ASSERT_NO_THROW(
        tp = std::make_unique<ThreadPool>(threads);
    );

    const int n = tp->runningThreads();
    
    assertLogContains(logFilePath, "INFO", "ThreadPool", "created with");
    assertLogDoesNotContain(logFilePath, "ERROR");
    EXPECT_EQ(n, ThreadPool::maxThreads());
}

TEST_F(ThreadPoolTest, InitWithNegativeOneValueHenseMaxThreads){
    const unsigned int threads = -1;
    std::unique_ptr<ThreadPool> tp;

    ASSERT_NO_THROW(
        tp = std::make_unique<ThreadPool>(threads);
    );

    const int n = tp->runningThreads();
    
    assertLogContains(logFilePath, "INFO", "ThreadPool", "created with");
    assertLogDoesNotContain(logFilePath, "ERROR");
    EXPECT_EQ(n, ThreadPool::maxThreads());
}

TEST_F(ThreadPoolTest, InitWithALargeValueHenseMaxThreads){
    const unsigned int threads = std::numeric_limits<int>::max();
    std::unique_ptr<ThreadPool> tp;

    ASSERT_NO_THROW(
        tp = std::make_unique<ThreadPool>(threads);
    );

    const int n = tp->runningThreads();
    
    assertLogContains(logFilePath, "INFO", "ThreadPool", "created with");
    assertLogDoesNotContain(logFilePath, "ERROR");
    EXPECT_EQ(n, ThreadPool::maxThreads());
}

TEST_F(ThreadPoolTest, RunSimpleTasksMaxThreads){
    int threads = 0;
    ThreadPool threadPool(threads);
    static int taskN = 0;
    auto func = [](){
        int currentTask{ taskN++ };
        DMX_LOG_DEBUG("ThreadPool", "Task ", currentTask, " is starting on ", std::this_thread::get_id(), " thread."); // Thread safe 
        // Processing task...
        DMX_LOG_DEBUG("ThreadPool", "Task ", currentTask, " sucessfully completed."); // Thread safe
    };
    std::vector<std::future<void>> futures;
    for(int i = 0; i < 4; ++i)
    {
        futures.push_back(threadPool.enqueue(func));
    }
    for (auto& f : futures)
        f.get();
    assertLogDoesNotContain(logFilePath, "ERROR");
}
