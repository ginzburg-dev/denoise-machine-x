#include <gtest/gtest.h>

#include <dmxdenoiser/StringConversions.hpp>

#include <string>
#include <string_view>
#include <vector>

using namespace dmxdenoiser;

// Mock types
struct Point2D { 
    int x{}; int y{}; 
};
struct Point3D { 
    int x{}; int y{}; int z{}; 
};
struct Point4D { 
    int x{}; int y{}; int z{}; int t{}; 
    std::string ToString() const { 
        return "Point4D: ToString() method";
    } 
};

// ToString()
constexpr std::string_view ToString(const Point2D& p) { return "Point2D info"; }
constexpr std::string_view ToString(const Point3D& p) { return "Point3D info"; }


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

TEST(StringConversions, HasToString) {
    EXPECT_TRUE(has_ToString_external_v<Point2D>);
    EXPECT_FALSE(has_ToString_external_v<Point4D>);

    EXPECT_TRUE(has_ToString_method_v<Point4D>);
    EXPECT_FALSE(has_ToString_method_v<Point2D>);

    //EXPECT_TRUE(has_ToString_external_v<PixelType>);
    //EXPECT_FALSE(has_ToString_external_v<Kernel2D>);

    //EXPECT_TRUE(has_ToString_method_v<Kernel2D>);
    //EXPECT_FALSE(has_ToString_method_v<PixelType>);
}

TEST(StringConversions, JoinVector) {
    std::vector<Point2D> vp2 { Point2D{}, Point2D{}, Point2D{} };
    std::vector<Point3D> vp3 { Point3D{}, Point3D{}, Point3D{} };
    std::vector<Point4D> vp4 { Point4D{}, Point4D{}, Point4D{} };
    std::vector<std::string> vs { "s1", "s2", "s3" };
    std::vector<float> vf { 0.01f, 0.1f, 1.0f };
    std::vector<int> vi { 1, 2, 3 };
    std::vector<bool> vb { true, false, true };

    EXPECT_EQ(joinVector(vp2), "[Point2D info, Point2D info, Point2D info]");
    EXPECT_EQ(joinVector(vp3), "[Point3D info, Point3D info, Point3D info]");
    EXPECT_EQ(joinVector(vp4), "[Point4D: ToString() method, Point4D: ToString() method, Point4D: ToString() method]");
    EXPECT_EQ(joinVector(vs), "[s1, s2, s3]");
    std::cout << joinVector(vf);
    EXPECT_EQ(joinVector(vi), "[1, 2, 3]");
    EXPECT_EQ(joinVector(vb), "[true, false, true]");
}

TEST(StringConversions, GetPadding) {
    EXPECT_EQ(getPadding("####", '#'), 4);
    EXPECT_EQ(getPadding("1#1##", '#'), 2);
    EXPECT_EQ(getPadding("#", '#'), 1);
    EXPECT_EQ(getPadding("none", '#'), 0);
    EXPECT_EQ(getPadding("######", '#'), 6);
}

TEST(StringConversions, IntToStringPadded) {
    EXPECT_EQ(intToStringPadded(1, 4), "0001");
    EXPECT_EQ(intToStringPadded(-1, 4), "-0001");
    EXPECT_EQ(intToStringPadded(10, 4), "0010");
    EXPECT_EQ(intToStringPadded(-10, 4), "-0010");
    EXPECT_EQ(intToStringPadded(100, 4), "0100");
    EXPECT_EQ(intToStringPadded(-100, 4), "-0100");
    EXPECT_EQ(intToStringPadded(1000, 4), "1000");
    EXPECT_EQ(intToStringPadded(-1000, 4), "-1000");
    EXPECT_EQ(intToStringPadded(100, 3), "100");
}

