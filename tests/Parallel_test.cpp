#include <gtest/gtest.h>

#include "AssertLogContains.hpp"
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/ThreadPool.hpp>
#include <dmxdenoiser/Parallel.hpp>

#include <atomic>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <limits>
#include <future>
#include <mutex>
#include <string>
#include <string_view>

using namespace dmxdenoiser;

class ParallelTest : public ::testing::Test {
    protected:
        std::string logFilePath = "../tests/test_files/dmxdenoiser_parallel_test.log";
        std::mutex tpMutex;

        void SetUp() override {
            bool success = std::filesystem::remove(logFilePath); // Remove log file
            DMX_LOG_INIT(LogLevel::Trace, &std::clog, logFilePath);
        }
};

TEST_F(ParallelTest, ParallelForOneParam){
    ThreadPool threadPool(-1);
    std::atomic<int> result = 0;
    int N = 200;
    parallelFor(0, N, [&](int i){
        ++result;
    }, &threadPool);

    EXPECT_EQ(N, result);
    assertLogDoesNotContain(logFilePath, "ERROR");
}

TEST_F(ParallelTest, ParallelForTwoParam){
    ThreadPool threadPool(-1);
    std::atomic<int> result = 0;
    int N = 200;
    parallelFor(0, N, [&](int s, int e){
        for (int i = s; i < e; ++i)
        {
            ++result;
        }
    }, &threadPool);
    EXPECT_EQ(N, result);
    assertLogDoesNotContain(logFilePath, "ERROR");
}

TEST_F(ParallelTest, ParallelForNoPool){
    std::atomic<int> result = 0;
    int N = 200;
    parallelFor(0, N, [&](int i){
        ++result;
    });

    EXPECT_EQ(N, result);
    assertLogDoesNotContain(logFilePath, "ERROR");
}

TEST_F(ParallelTest, ParallelForError) {
    ThreadPool threadPool(-1);
    std::atomic<int> result = 0;
    int N = 200;
    try
    {
        parallelFor(0, N, [&](int i){
            throw std::runtime_error("Some error."); 
        }, &threadPool);
    }
    catch(const std::exception& e)
    {
        EXPECT_NE(std::string(e.what()).find("Some error."), std::string::npos);
    }

    assertLogContains(logFilePath, "ERROR", "Some error.");
}

TEST_F(ParallelTest, ParallelForChunkStep2N300){
    std::unique_ptr<ThreadPool> threadPool = std::make_unique<ThreadPool>(2);
    std::atomic<int> result = 0;
    int N = 300;
    int chunk_size = 2;
    parallelFor(0, N, [&](int x, int y){
        ++result;
    }, threadPool.get(), chunk_size);
    EXPECT_EQ( N / chunk_size, result);
    assertLogDoesNotContain(logFilePath, "ERROR");
}

TEST_F(ParallelTest, ParallelForChunkStep0N400){
    std::unique_ptr<ThreadPool> threadPool = std::make_unique<ThreadPool>(2);
    std::atomic<int> result = 0;
    int N = 400;
    int chunk_size = (N / (8 * threadPool->runningThreads()));
    parallelFor(0, N, [&](int x, int y){
        ++result;
    }, threadPool.get(), 0); // Zero chunk
    EXPECT_EQ( N / chunk_size , result);
    assertLogDoesNotContain(logFilePath, "ERROR");
}

TEST_F(ParallelTest, ParallelForChunkStepInfN2345){
    std::unique_ptr<ThreadPool> threadPool = std::make_unique<ThreadPool>(2);
    std::atomic<int> result = 0;
    int N = 2345;
    int chunk_size = (N / (8 * threadPool->runningThreads()));
    parallelFor(0, N, [&](int x, int y){
        ++result;
    }, threadPool.get(), std::numeric_limits<unsigned int>::max());
    EXPECT_EQ( N / std::min<unsigned int>(std::numeric_limits<unsigned int>::max(), N) , result);
    const std::uint64_t N64 = static_cast<std::uint64_t>(N);
    const std::uint64_t UMAX = static_cast<std::uint64_t>(std::numeric_limits<unsigned int>::max());
    const std::uint64_t denom = std::min<std::uint64_t>(N64, UMAX);
    
    DMX_LOG_TRACE("ParallelTest",
        "ParallelForChunkStepInfN2345: N / min(N, UINT_MAX) = ", N64 / denom);

    assertLogDoesNotContain(logFilePath, "ERROR");
}
