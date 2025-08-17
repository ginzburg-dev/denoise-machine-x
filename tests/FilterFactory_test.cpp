#include <gtest/gtest.h>

#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/FilterFactory.hpp>

#include <iostream>
#include <memory>
#include <string>

using namespace dmxdenoiser;

TEST(FilterFactory, CanCreateRegisteredFilter){
    auto  exampleFilter = FilterFactory::instance().create("ConvolutionFilter");
    ASSERT_NE(exampleFilter, nullptr);
    std::string exampleFilterString = exampleFilter->ToString();
    EXPECT_TRUE(exampleFilterString.find("ConvolutionFilter:") != std::string::npos);
    std::cout << exampleFilterString << '\n';
}

TEST(FilterFactory, CanCreateRegisteredFilterWithDefinition){
    auto  exampleFilter = DMX_CREATE_FILTER("ConvolutionFilter");
    ASSERT_NE(exampleFilter, nullptr);
    std::string exampleFilterString = exampleFilter->ToString();
    EXPECT_TRUE(exampleFilterString.find("ConvolutionFilter:") != std::string::npos);
    std::cout << exampleFilterString << '\n';
}

TEST(FilterFactory, ThrowOnUnknownFilter){
    std::string logFilePath = "../tests/test_files/dmxdenoiser_filterFactory_test.log";
    bool success = std::filesystem::remove(logFilePath); // Remove log file
    DMX_LOG_INIT(LogLevel::Debug, &std::clog, logFilePath);

    EXPECT_THROW({FilterFactory::instance().create("Unknown");}, std::runtime_error);

    // Check log file
    std::string tag{"FilterFactory"};
    std::string msg{"Filter not registered:"};
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

TEST(FilterFactory, ReturnUniquePtrEachTime){
    auto f1 = FilterFactory::instance().create("ConvolutionFilter");
    auto f2 = FilterFactory::instance().create("ConvolutionFilter");
    ASSERT_NE(f1, nullptr);
    ASSERT_NE(f2, nullptr);
    EXPECT_NE(f1.get(), f2.get());
}

TEST(FilterFactory, CanOverrideFilterRegistration){
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
            std::string ToString() const override { return "SpectralFilter"; };

        protected:
            void resetParams() override {};

        private:
            void applyFilter(const DMXImage& in, DMXImage& out) const override {};
        };
        return std::make_unique<SpectralFilter>();
    });
    auto filter = FilterFactory::instance().create("ConvolutionFilter");
    ASSERT_NE(filter, nullptr);
    EXPECT_STREQ(filter->Name(), "SpectralFilter");
    EXPECT_EQ(filter->ToString(), "SpectralFilter");
}
