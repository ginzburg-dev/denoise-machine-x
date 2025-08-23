#include <gtest/gtest.h>

#include "TestConfig.hpp"
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
    std::string getLogPath(std::string_view testDir = TEST_LOG_DIR) {
        auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
        return std::string(testDir) + info->test_suite_name() + "/" + info->name() + ".log";
    }

    void SetUp() override {
        removeLogFile();
        DMX_LOG_INIT(LogLevel::Trace, &std::clog, this->getLogPath());
    }

    void TearDown() override {
        DMX_LOG_SHUTDOWN;
        //removeLogFile();
    }

    void removeLogFile() {
        bool success = std::filesystem::remove(this->getLogPath()); // Remove log file
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
    assertLogDoesNotContain(getLogPath(), "ERROR");
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
    assertLogDoesNotContain(getLogPath(), "ERROR");
}

TEST_F(ParallelTest, ParallelForNoPool){
    std::atomic<int> result = 0;
    int N = 200;
    parallelFor(0, N, [&](int i){
        ++result;
    });

    EXPECT_EQ(N, result);
    assertLogDoesNotContain(getLogPath(), "ERROR");
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

    assertLogContains(getLogPath(), "ERROR", "Some error.");
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
    assertLogDoesNotContain(getLogPath(), "ERROR");
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
    assertLogDoesNotContain(getLogPath(), "ERROR");
}

TEST_F(ParallelTest, ParallelForChunkStepInfN2345){
    std::unique_ptr<ThreadPool> threadPool = std::make_unique<ThreadPool>(2);
    std::atomic<int> result = 0;
    int N = 2345;
    int chunk_size = (N / (8 * threadPool->runningThreads()));
    parallelFor(0, N, [&](int x, int y){
        ++result;
    }, threadPool.get(), std::numeric_limits<int>::max());
    EXPECT_EQ( N / std::min<int>(std::numeric_limits<int>::max(), N) , result);
    const int N64 = static_cast<int>(N);
    const int UMAX = static_cast<int>(std::numeric_limits<int>::max());
    const int denom = std::min<int>(N64, UMAX);
    
    DMX_LOG_TRACE("ParallelTest",
        "ParallelForChunkStepInfN2345: N / min(N, UINT_MAX) = ", N64 / denom);

    assertLogDoesNotContain(getLogPath(), "ERROR");
}
