#include <gtest/gtest.h>

#include "TestConfig.hpp"
#include "AssertLogContains.hpp"
#include <dmxdenoiser/Parser.hpp>
#include <string>

using namespace dmxdenoiser;

class ParserTest : public ::testing::Test {
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

TEST_F(ParserTest, ParseArgs)
{
    const char* argv[] = { 
        "./dmxdenoiser",
        "--gpu", "1",
        "--ncores", "10",
        "--start", "1",
        "--end", "100",
        "--input", "./input",
        "--output", "./output",
        "--config", "./config/filter.json",
        "--beauty", "image.####.exr",
        "--albedo", "image.####.exr:albedo",
        "--normal", "image.####.exr:normal",
        "--depth", "image_depth.####.exr:depth",
    };
    int argc = 23;
    Options opt = parseArguments(argc,const_cast<char**>(argv));
    EXPECT_EQ(opt.gpu, true);
    EXPECT_EQ(opt.ncores, 10);
    EXPECT_EQ(opt.startFrame, 1);
    EXPECT_EQ(opt.endFrame, 100);
    EXPECT_EQ(opt.inputFolder, std::string("./input"));
    EXPECT_EQ(opt.outputFolder, std::string("./output"));
}
