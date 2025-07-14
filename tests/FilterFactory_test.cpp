#include <gtest/gtest.h>

#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/filters/ExampleFilter.hpp>

#include <iostream>
#include <memory>
#include <string>

using namespace dmxdenoiser;

TEST(FilterFactory, CanCreateRegisteredFilter){
    auto  exampleFilter = FilterFactory::instance().create("ExampleFilter");
    ASSERT_NE(exampleFilter, nullptr);
    std::string exampleFilterString = exampleFilter->ToString();
    EXPECT_NE(exampleFilterString.find("ExampleFilter:"), std::string::npos);
    std::cout << exampleFilterString << '\n';
}

TEST(FilterFactory, ThrowOnUnknownFilter){
    EXPECT_THROW({FilterFactory::instance().create("Unknown");}, std::runtime_error);
}

TEST(FilterFactory, ReturnUniquePtrEachTime){
    auto f1 = FilterFactory::instance().create("ExampleFilter");
    auto f2 = FilterFactory::instance().create("ExampleFilter");
    ASSERT_NE(f1, nullptr);
    ASSERT_NE(f2, nullptr);
    EXPECT_NE(f1.get(), f2.get());
}

TEST(FilterFactory, CanOverrideFilterRegistration){
    FilterFactory::instance().registerFilter("ExampleFilter", [](){
        class SpectralFilter : public Filter
        {
        public:
            // Required: unique filter name
            static constexpr const char* StaticClassName() { return "SpectralFilter"; }
            const char* Name() const override { return StaticClassName(); };
            SpectralFilter() = default;
            ~SpectralFilter() override = default;
            void setParams(const ParamDictionary& params) override {};
            void apply(DMXImage& img) const override {};
            std::string ToString() const override { return "SpectralFilter"; };
        };
        return std::make_unique<SpectralFilter>();
    });
    auto filter = FilterFactory::instance().create("ExampleFilter");
    ASSERT_NE(filter, nullptr);
    EXPECT_STREQ(filter->Name(), "SpectralFilter");
    EXPECT_EQ(filter->ToString(), "SpectralFilter");
}
