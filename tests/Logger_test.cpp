#include <gtest/gtest.h>

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
        dmxdenoiser::Logger::instance().init(dmxdenoiser::LogLevel::Debug, &std::cout, path);
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
    Logger::instance().init(LogLevel::Debug, &std::clog, logFilePath);
    std::string tag = "ConvolutionFilter";
    std::string msg = "Filter kernel error";
    Logger::instance().Log(LogLevel::Error, tag, msg);

    // Check log file
    ASSERT_TRUE(std::filesystem::exists(logFilePath));
    ASSERT_GT(std::filesystem::file_size(logFilePath), 0u);
    std::ifstream ifile{logFilePath};
    ASSERT_TRUE(ifile.good());
    std::string line{};
    ASSERT_TRUE(static_cast<bool>(std::getline(ifile, line)));
    EXPECT_NE(line.find("ERROR"), std::string::npos);
    EXPECT_NE(line.find(tag), std::string::npos);
    EXPECT_NE(line.find(msg), std::string::npos);
}

TEST(Logger, LoggerInitWithDefinition)
{
    std::string logFilePath = "../tests/test_files/dmxdenoiser_logger_test.log";
    bool success = std::filesystem::remove(logFilePath); // Remove log file
    DMX_LOG_INIT(LogLevel::Debug, &std::clog, logFilePath);
    std::string tag = "ConvolutionFilter";
    std::string msg = "Filter kernel error";
    Logger::instance().Log(LogLevel::Error, tag, msg);

    // Check log file
    ASSERT_TRUE(std::filesystem::exists(logFilePath));
    ASSERT_GT(std::filesystem::file_size(logFilePath), 0u);
    std::ifstream ifile{logFilePath};
    ASSERT_TRUE(ifile.good());
    std::string line{};
    ASSERT_TRUE(static_cast<bool>(std::getline(ifile, line)));
    EXPECT_NE(line.find("ERROR"), std::string::npos);
    EXPECT_NE(line.find(tag), std::string::npos);
    EXPECT_NE(line.find(msg), std::string::npos);
}

TEST(Logger, LoggerDefinition)
{
    std::string logFilePath = "../tests/test_files/dmxdenoiser_logger_test.log";
    bool success = std::filesystem::remove(logFilePath); // Remove log file

    Logger::instance().init(LogLevel::Debug, &std::clog, logFilePath);

    std::string tag = "Filter";
    std::string msg = "Strength=";
    float strength = 10.0f;
    DMX_LOG_DEBUG(tag, msg, strength);
    DMX_LOG_INFO(tag, msg, strength);
    DMX_LOG_WARNING(tag, msg, strength);
    DMX_LOG_ERROR(tag, msg, strength);

    // Check log file
    ASSERT_TRUE(std::filesystem::exists(logFilePath));
    ASSERT_GT(std::filesystem::file_size(logFilePath), 0u);
    std::ifstream ifile{logFilePath};
    ASSERT_TRUE(ifile.good());
    std::string line{};

    // DEBUG
    ASSERT_TRUE(static_cast<bool>(std::getline(ifile, line)));
    EXPECT_NE(line.find("DEBUG"), std::string::npos);
    EXPECT_NE(line.find(tag), std::string::npos);
    EXPECT_NE(line.find(msg), std::string::npos);

    // INFO
    ASSERT_TRUE(static_cast<bool>(std::getline(ifile, line)));
    EXPECT_NE(line.find("INFO"), std::string::npos);
    EXPECT_NE(line.find(tag), std::string::npos);
    EXPECT_NE(line.find(msg), std::string::npos);

    // WARNING
    ASSERT_TRUE(static_cast<bool>(std::getline(ifile, line)));
    EXPECT_NE(line.find("WARNING"), std::string::npos);
    EXPECT_NE(line.find(tag), std::string::npos);
    EXPECT_NE(line.find(msg), std::string::npos);

    // ERROR
    ASSERT_TRUE(static_cast<bool>(std::getline(ifile, line)));
    EXPECT_NE(line.find("ERROR"), std::string::npos);
    EXPECT_NE(line.find(tag), std::string::npos);
    EXPECT_NE(line.find(msg), std::string::npos);
}
