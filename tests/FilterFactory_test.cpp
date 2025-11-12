#include <gtest/gtest.h>

#include "TestConfig.hpp"
#include "AssertLogContains.hpp"
#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/FilterFactory.hpp>

#include <iostream>
#include <memory>
#include <string>

using namespace dmxdenoiser;

class FilterFactoryTest : public ::testing::Test {
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

TEST_F(FilterFactoryTest, CanCreateRegisteredFilter){
    auto  exampleFilter = FilterFactory::instance().create("ConvolutionFilter");
    ASSERT_NE(exampleFilter, nullptr);
    std::string exampleFilterString = exampleFilter->ToString();
    EXPECT_TRUE(exampleFilterString.find("ConvolutionFilter:") != std::string::npos);
    std::cout << exampleFilterString << '\n';
}

TEST_F(FilterFactoryTest, CanCreateRegisteredFilterWithDefinition){
    auto  exampleFilter = DMX_CREATE_FILTER("ConvolutionFilter");
    ASSERT_NE(exampleFilter, nullptr);
    std::string exampleFilterString = exampleFilter->ToString();
    EXPECT_TRUE(exampleFilterString.find("ConvolutionFilter:") != std::string::npos);
    std::cout << exampleFilterString << '\n';
}

TEST_F(FilterFactoryTest, ThrowOnUnknownFilter){
    EXPECT_THROW({FilterFactory::instance().create("Unknown");}, std::runtime_error);

    // Check log file
    std::string tag{"FilterFactory"};
    std::string msg{"Filter not registered:"};
    assertLogContains(getLogPath(), "ERROR", tag, msg);
}

TEST_F(FilterFactoryTest, ReturnUniquePtrEachTime){
    auto f1 = FilterFactory::instance().create("ConvolutionFilter");
    auto f2 = FilterFactory::instance().create("ConvolutionFilter");
    ASSERT_NE(f1, nullptr);
    ASSERT_NE(f2, nullptr);
    EXPECT_NE(f1.get(), f2.get());
}

TEST_F(FilterFactoryTest, CanOverrideFilterRegistration){
    FilterFactory::instance().registerFilter("ConvolutionFilter", [](){
        class SpectralFilter : public Filter
        {
        public:
            // Required: unique filter name
            static constexpr const char* StaticClassName() { return "SpectralFilter"; }
            const char* Name() const override { return StaticClassName(); };
            SpectralFilter() = default;
            ~SpectralFilter() override = default;
            void setParams(const ParamDictionary& params) override {};

        protected:
            void resetParams() override {};

            virtual void runFilterCPU(
                const DMXImage& input,
                DMXImage& output,
                const std::vector<int>& layers,
                const std::vector<int>& frames
            ) const override {};
        };
        return std::make_unique<SpectralFilter>();
    });
    auto filter = FilterFactory::instance().create("ConvolutionFilter");
    ASSERT_NE(filter, nullptr);
    EXPECT_STREQ(filter->Name(), "SpectralFilter");
}
