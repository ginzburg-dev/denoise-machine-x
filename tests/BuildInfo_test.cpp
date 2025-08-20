#include <gtest/gtest.h>

#include <dmxdenoiser/BuildInfo.hpp>
#include <dmxdenoiser/Logger.hpp>

#include <filesystem>

using namespace dmxdenoiser;

class BuildInfoTest : public ::testing::Test {
protected:
    std::string logFilePath = "../tests/test_files/dmxdenoiser_build_info_test.log";
    
    void SetUp() override {
        bool success = std::filesystem::remove(logFilePath); // Remove log file
        DMX_LOG_INIT(LogLevel::Trace, &std::clog, logFilePath);
    }
};

TEST_F(BuildInfoTest, LogBanner){
    logBuildBanner();
}
