#include <gtest/gtest.h>

#include "TestConfig.hpp"
#include <dmxdenoiser/BuildInfo.hpp>
#include <dmxdenoiser/Logger.hpp>

#include <filesystem>
#include <string>
#include <string_view>

using namespace dmxdenoiser;

class BuildInfoTest : public ::testing::Test {
protected:
    std::string getLogPath(std::string_view testDir = TEST_LOG_DIR) {
        auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
        return std::string(testDir) + info->test_suite_name() + "/" + info->name() + ".log";
    }

    void SetUp() override {
        removeLogFile();
        DMX_LOG_INIT(DMX_MIN_LOG_LEVEL, &std::clog, this->getLogPath());
    }

    void TearDown() override {
        DMX_LOG_SHUTDOWN;
        //removeLogFile();
    }

    void removeLogFile() {
        bool success = std::filesystem::remove(this->getLogPath()); // Remove log file
    }
};

TEST_F(BuildInfoTest, LogBanner){
    logBuildBanner();
}
