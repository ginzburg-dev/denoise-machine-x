#include <gtest/gtest.h>

#include <dmxdenoiser/ParamDictionary.hpp>

#include <string>
#include <vector>

using namespace dmxdenoiser;

TEST(ParamDictionary, SetAndGetIntSingleValue){
    ParamDictionary params;
    int value = 7;
    params.addInt("sample", value);
    auto sample_param = params.getSingleParam<int>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST(ParamDictionary, SetAndGetFloatSingleValue){
    ParamDictionary params;
    float value = 7.56789f;
    params.addFloat("sample", value);
    auto sample_param = params.getSingleParam<float>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST(ParamDictionary, SetAndGetBoolSingleValue){
    ParamDictionary params;
    bool value = true;
    params.addBool("sample", value);
    auto sample_param = params.getSingleParam<bool>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST(ParamDictionary, SetAndGetStringSingleValue){
    ParamDictionary params;
    std::string value = "SampleString";
    params.addString("sample", value);
    auto sample_param = params.getSingleParam<std::string>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST(ParamDictionary, GetMissingParamReturnsNullopt){
    ParamDictionary params;
    params.addInt("sample", 7);
    auto sample_param = params.getSingleParam<int>("nonexistent_key");
    EXPECT_EQ(sample_param, std::nullopt);
}

TEST(ParamDictionary, GetIntForStringParamReturnsNullopt){
    ParamDictionary params;
    params.addString("sample", "StringParam");
    auto sample_param = params.getSingleParam<int>("sample");
    EXPECT_EQ(sample_param, std::nullopt);
}

TEST(ParamDictionary, SetAndGetIntArrayValue){
    ParamDictionary params;
    std::vector<int> value = { 7, 8 };
    params.addIntArray("sample", value);
    auto sample_param = params.getArrayParam<int>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST(ParamDictionary, SetAndGetFloatArrayValue){
    ParamDictionary params;
    std::vector<float> value = { 7.5f, 8.9f };
    params.addFloatArray("sample", value);
    auto sample_param = params.getArrayParam<float>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST(ParamDictionary, SetAndGetBoolArrayValue){
    ParamDictionary params;
    std::vector<bool> value = { true, false };
    params.addBoolArray("sample", value);
    auto sample_param = params.getArrayParam<bool>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST(ParamDictionary, SetAndGetStringArrayValue){
    ParamDictionary params;
    std::vector<std::string> value = { "First", "Second" };
    params.addStringArray("sample", value);
    auto sample_param = params.getArrayParam<std::string>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST(ParamDictionary, GetMissingStringArrayParamReturnsNullopt){
    ParamDictionary params;
    std::vector<std::string> value = { "First", "Second" };
    params.addStringArray("sample", value);
    auto sample_param = params.getArrayParam<std::string>("nonexistance_key");
    EXPECT_EQ(sample_param, std::nullopt);
}

TEST(ParamDictionary, GetFloatForStringArrayParamReturnsNullopt){
    ParamDictionary params;
    std::vector<std::string> value = { "First", "Second" };
    params.addStringArray("sample", value);
    auto sample_param = params.getArrayParam<float>("sample");
    EXPECT_EQ(sample_param, std::nullopt);
}

TEST(ParamDictionary, AddEmptyArray){
    ParamDictionary params;
    std::vector<int> empty;
    params.addIntArray("empty", empty);
    auto val = params.getArrayParam<int>("empty");
    ASSERT_TRUE(val.has_value());
    EXPECT_TRUE(val->empty());
    auto single = params.getSingleParam<int>("empty");
    EXPECT_EQ(single, std::nullopt);
}

TEST(ParamDictionary, OverwriteIntParam){
    ParamDictionary params;
    params.addInt("x", 1);
    params.addInt("x", 2); // Should overwrite
    auto val = params.getSingleParam<int>("x");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 2);
}
