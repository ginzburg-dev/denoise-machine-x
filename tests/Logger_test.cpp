#include <gtest/gtest.h>

#include "TestConfig.hpp"
#include "AssertLogContains.hpp"
#include <dmxdenoiser/Logger.hpp>

#include <cstdio>
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>

using namespace dmxdenoiser;

class LoggerTest : public ::testing::Test {
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

template<typename... Args>
void func(Args&&... args)
{
    ((std::cout << std::forward<Args>(args) << ' '), ...) << '\n';
    (std::cout << ... << std::forward<Args>(args)) << '\n';
}

TEST_F(LoggerTest, VardicFuncTemplates)
{
    func(1, 2, "ggg", "sss");
}

TEST_F(LoggerTest, LoggerInit)
{
    std::string tag = "ConvolutionFilter";
    std::string msg = "Filter kernel error";
    Logger::instance().Log(LogLevel::Error, tag, msg);

    // Check log file
    assertLogContains(getLogPath(), "ERROR", tag, msg);
}

TEST_F(LoggerTest, LoggerInitWithDefinition)
{
    std::string tag = "ConvolutionFilter";
    std::string msg = "Filter kernel error";
    Logger::instance().Log(LogLevel::Error, tag, msg);

    // Check log file
    assertLogContains(getLogPath(), "ERROR", tag, msg);
}

TEST_F(LoggerTest, LoggerDefinition)
{
    std::string tag = "Filter";
    std::string msg = "Strength=";
    float strength = 10.0f;
    
    DMX_LOG_TRACE(tag, msg, strength);
    DMX_LOG_DEBUG(tag, msg, strength);
    DMX_LOG_INFO(tag, msg, strength);
    DMX_LOG_WARNING(tag, msg, strength);
    DMX_LOG_ERROR(tag, msg, strength);
    DMX_LOG_CRITICAL(tag, msg, strength);

    // Check log file
    #if DMX_DEBUG_BUILD
        assertLogContains(getLogPath(), "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", tag, msg, "10");
    #else
        assertLogContains(getLogPath(),  "INFO", "WARNING", "ERROR", "CRITICAL", tag, msg, "10");
    #endif
}
