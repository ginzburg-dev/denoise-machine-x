#include <gtest/gtest.h>

#include "TestConfig.hpp"
#include <dmxdenoiser/Backend.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/Kernel2D.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>

#include <string>
#include <vector>

using namespace dmxdenoiser;

class ParamDictionaryTest : public ::testing::Test {
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

TEST_F(ParamDictionaryTest, SetAndGetIntSingleValue){
    ParamDictionary params;
    int value = 7;
    params.addInt("sample", value);
    auto sample_param = params.getSingleParam<int>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetFloatSingleValue){
    ParamDictionary params;
    float value = 7.56789f;
    params.addFloat("sample", value);
    auto sample_param = params.getSingleParam<float>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetBoolSingleValue){
    ParamDictionary params;
    bool value = true;
    params.addBool("sample", value);
    auto sample_param = params.getSingleParam<bool>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetStringSingleValue){
    ParamDictionary params;
    std::string value = "SampleString";
    params.addString("sample", value);
    auto sample_param = params.getSingleParam<std::string>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetKernel2DSingleValue){
    ParamDictionary params;
    Kernel2D value = FilterKernels::getGaussianKernel(3, 1);
    params.addKernel2D("sample", value);
    auto sample_param = params.getSingleParam<Kernel2D>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetBackendSingleValue){
    ParamDictionary params;
    Backend value = Backend::GPU;
    params.addBackend("sample", value);
    auto sample_param = params.getSingleParam<Backend>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetBackendResourceSingleValue){
    ParamDictionary params;
    BackendResource value = BackendResource{};
    params.addBackendResource("sample", value);
    auto sample_param = params.getSingleParam<BackendResource>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sizeof(sample_param.value()), sizeof(value));
}

TEST_F(ParamDictionaryTest, GetMissingParamReturnsNullopt){
    ParamDictionary params;
    params.addInt("sample", 7);
    auto sample_param = params.getSingleParam<int>("nonexistent_key");
    EXPECT_EQ(sample_param, std::nullopt);
}

TEST_F(ParamDictionaryTest, GetIntForStringParamReturnsNullopt){
    ParamDictionary params;
    params.addString("sample", "StringParam");
    auto sample_param = params.getSingleParam<int>("sample");
    EXPECT_EQ(sample_param, std::nullopt);
}

TEST_F(ParamDictionaryTest, SetAndGetIntArrayValue){
    ParamDictionary params;
    std::vector<int> value = { 7, 8 };
    params.addIntArray("sample", value);
    auto sample_param = params.getArrayParam<int>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetFloatArrayValue){
    ParamDictionary params;
    std::vector<float> value = { 7.5f, 8.9f };
    params.addFloatArray("sample", value);
    auto sample_param = params.getArrayParam<float>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetBoolArrayValue){
    ParamDictionary params;
    std::vector<bool> value = { true, false };
    params.addBoolArray("sample", value);
    auto sample_param = params.getArrayParam<bool>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetStringArrayValue){
    ParamDictionary params;
    std::vector<std::string> value = { "First", "Second" };
    params.addStringArray("sample", value);
    auto sample_param = params.getArrayParam<std::string>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetKernel2DArrayValue){
    ParamDictionary params;
    std::vector<Kernel2D> value = { FilterKernels::getGaussianKernel(3, 1), FilterKernels::getBoxKernel(3) };
    params.addKernel2DArray("sample", value);
    auto sample_param = params.getArrayParam<Kernel2D>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetBackendArrayValue){
    ParamDictionary params;
    std::vector<Backend> value = { Backend::GPU, Backend::METAL };
    params.addBackendArray("sample", value);
    auto sample_param = params.getArrayParam<Backend>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sample_param.value(), value);
}

TEST_F(ParamDictionaryTest, SetAndGetBackendResourceArrayValue){
    ParamDictionary params;
    std::vector<BackendResource> value = { BackendResource{}, BackendResource{} };
    params.addBackendResourceArray("sample", value);
    auto sample_param = params.getArrayParam<BackendResource>("sample");
    ASSERT_TRUE(sample_param.has_value());
    EXPECT_EQ(sizeof(sample_param.value()), sizeof(value));
}

TEST_F(ParamDictionaryTest, GetMissingStringArrayParamReturnsNullopt){
    ParamDictionary params;
    std::vector<std::string> value = { "First", "Second" };
    params.addStringArray("sample", value);
    auto sample_param = params.getArrayParam<std::string>("nonexistance_key");
    EXPECT_EQ(sample_param, std::nullopt);
}

TEST_F(ParamDictionaryTest, GetFloatForStringArrayParamReturnsNullopt){
    ParamDictionary params;
    std::vector<std::string> value = { "First", "Second" };
    params.addStringArray("sample", value);
    auto sample_param = params.getArrayParam<float>("sample");
    EXPECT_EQ(sample_param, std::nullopt);
}

TEST_F(ParamDictionaryTest, AddEmptyArray){
    ParamDictionary params;
    std::vector<int> empty;
    params.addIntArray("empty", empty);
    auto val = params.getArrayParam<int>("empty");
    ASSERT_TRUE(val.has_value());
    EXPECT_TRUE(val->empty());
    auto single = params.getSingleParam<int>("empty");
    EXPECT_EQ(single, std::nullopt);
}

TEST_F(ParamDictionaryTest, OverwriteIntParam){
    ParamDictionary params;
    params.addInt("x", 1);
    params.addInt("x", 2); // Should overwrite
    auto val = params.getSingleParam<int>("x");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 2);
}

TEST_F(ParamDictionaryTest, ToString){
    ParamDictionary params;
    params.addInt("int_var", 1);
    params.addFloat("float_var", 2.0f);
    params.addBool("bool_var", true);
    params.addString("string_var", "String data");
    params.addKernel2D("kernel_var", FilterKernels::getGaussianKernel(3, 1));
    params.addBackend("backend_var", Backend::GPU);
    params.addBackendResource("backend_res_var", BackendResource{});

    params.addIntArray("int_array_var", {0, 2});
    params.addFloatArray("float_array_var", {3.0f, 4.0f});
    params.addBoolArray("bool_array_var", {false, true});
    params.addStringArray("string_array_var", {"String data1", "String data2"});
    params.addKernel2DArray("kernel_array_var", {FilterKernels::getBoxKernel(7), FilterKernels::getBoxKernel(5)});
    params.addBackendArray("backend_array_var", {Backend::GPU, Backend::METAL});
    params.addBackendResourceArray("backend_res_array_var", {BackendResource{}, BackendResource{}});
    DMX_LOG_INFO("ParamDictionary", params.ToString());
}
