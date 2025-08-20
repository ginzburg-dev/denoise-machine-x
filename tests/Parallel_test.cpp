#include <gtest/gtest.h>

#include "AssertLogContains.hpp"
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/ThreadPool.hpp>
#include <dmxdenoiser/Parallel.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <mutex>
#include <string>
#include <string_view>

using namespace dmxdenoiser;

std::string logFilePath = "../tests/test_files/dmxdenoiser_parallel_test.log";
bool success = std::filesystem::remove(logFilePath); // Remove log file
std::mutex tpMutex;

TEST(Parallel, ParallelForSimple){
    DMX_LOG_INIT(LogLevel::Debug, &std::clog, logFilePath);
    ThreadPool threadPool(-1);
    int result = 0;
    int N = 10;
    parallelFor(0, N, threadPool, [&](int i){
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        DMX_LOG_DEBUG("TEST(Parallel, ParallelForSimple)", "Thread ", std::this_thread::get_id(), " completed task.");
        ++result;
    });

    EXPECT_EQ(N, result);
    //assertLogContains(logFilePath, "DEBUG", "ThreadPool", "created with");
}

TEST(Parallel, ParallelForError){
    ThreadPool threadPool(-1);
    int result = 0;
    int N = 10;
    EXPECT_THROW(
        parallelFor(0, N, threadPool, [&](int i){
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        throw std::runtime_error("Some error.");
        DMX_LOG_DEBUG("TEST(Parallel, ParallelForSimple)", "Thread ", std::this_thread::get_id(), " completed task.");
        ++result;}), 
        std::runtime_error
    );
    //assertLogContains(logFilePath, "DEBUG", "ThreadPool", "created with");
}
