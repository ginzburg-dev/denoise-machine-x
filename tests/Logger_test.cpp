#include <gtest/gtest.h>

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
    void SetUp() override {
        std::string path = "../tests/test_files/dmxdenoiser_logger_test.log";
        dmxdenoiser::Logger::instance().init(dmxdenoiser::LogLevel::Trace, &std::cout, path);
    }
};

template<typename... Args>
void func(Args&&... args)
{
    ((std::cout << std::forward<Args>(args) << ' '), ...) << '\n';
    (std::cout << ... << std::forward<Args>(args)) << '\n';
}

TEST(Logger, VardicFuncTemplates)
{
    func(1, 2, "ggg", "sss");
}

TEST(Logger, LoggerInit)
{
    std::string logFilePath = "../tests/test_files/dmxdenoiser_logger_test.log";
    bool success = std::filesystem::remove(logFilePath); // Remove log file
    Logger::instance().init(LogLevel::Trace, &std::clog, logFilePath);
    std::string tag = "ConvolutionFilter";
    std::string msg = "Filter kernel error";
    Logger::instance().Log(LogLevel::Error, tag, msg);

    // Check log file
    assertLogContains(logFilePath, "ERROR", tag, msg);
}

TEST(Logger, LoggerInitWithDefinition)
{
    std::string logFilePath = "../tests/test_files/dmxdenoiser_logger_test.log";
    bool success = std::filesystem::remove(logFilePath); // Remove log file
    DMX_LOG_INIT(LogLevel::Trace, &std::clog, logFilePath);
    std::string tag = "ConvolutionFilter";
    std::string msg = "Filter kernel error";
    Logger::instance().Log(LogLevel::Error, tag, msg);

    // Check log file
    assertLogContains(logFilePath, "ERROR", tag, msg);
}

TEST(Logger, LoggerDefinition)
{
    std::string logFilePath = "../tests/test_files/dmxdenoiser_logger_test.log";
    bool success = std::filesystem::remove(logFilePath); // Remove log file

    Logger::instance().init(LogLevel::Trace, &std::clog, logFilePath);

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
        assertLogContains(logFilePath, "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", tag, msg, "10");
    #else
        assertLogContains(logFilePath,  "INFO", "WARNING", "ERROR", "CRITICAL", tag, msg, "10");
    #endif
}
