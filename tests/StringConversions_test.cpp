#include <gtest/gtest.h>

#include <dmxdenoiser/StringConversions.hpp>

#include <string>

using namespace dmxdenoiser;

TEST(StringConversions, ConvertFloatToStringPreserveOrigDecimalPart){
    float f = 0.75f;
    std::string str = floatToString(f);
    // Should output "0.75" (no trailing zeros)
    EXPECT_EQ("0.75", str);
}

TEST(StringConversions, PassDoubleToConvert){
    double d = 0.12345678;
    std::string str = floatToString(d); // use default precision = 8
    EXPECT_EQ("0.12345678", str);
}

TEST(StringConversions, SmallValue) {
    double d = 0.00000012;
    std::string str = floatToString(d, 8);
    EXPECT_EQ("0.00000012", str);
}

TEST(StringConversions, LargeValue) {
    double d = 12345678.9;
    std::string str = floatToString(d, 1);
    EXPECT_EQ("12345678.9", str);
}

TEST(StringConversions, RoundsCorrectly) {
    double d = 1.23456789;
    std::string str = floatToString(d, 3);
    EXPECT_EQ("1.235", str);
}

TEST(StringConversions, IntegerValue) {
    double d = 2.0;
    std::string str = floatToString(d);
    EXPECT_EQ("2", str);
}
