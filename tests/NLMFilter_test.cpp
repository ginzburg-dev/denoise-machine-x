#include <gtest/gtest.h>

#include "TestConfig.hpp"
#include "AssertLogContains.hpp"
#include <dmxdenoiser/Backend.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/filters/NLMFilter.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/StringConversions.hpp>
#include <dmxdenoiser/ThreadPool.hpp>
#include <dmxdenoiser/ImageIOExr.hpp>

#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <memory>
#include <thread>

using namespace dmxdenoiser;

class NLMFilterTest : public ::testing::Test {
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

void applyFilterToImageFile(
    const std::string& filenameBeauty,
    const std::string& filenameAlbedo,
    const std::string& filenameNormal,
    const std::string& filenameDepth,
    const std::string& outputFileName,
    int radius,
    int patchRadius,
    float sigmaBeauty,
    float sigmaAlbedo,
    float sigmaNormal,
    float sigmaDepth,
    ThreadPool* pool = nullptr,
    AovDictionary aovs = { 
        {"beauty", "default"},
        {"albedo", "default"},
        {"normal", "default"},
        {"depth", "default"}
    },
    Backend backend=Backend::CPU,
    bool filterAllLayers = false
    )
{
    std::unique_ptr<ImageIO> io = ImageIO::create(filenameBeauty);
    auto info = io->getImageInfo(filenameBeauty);
    for (const auto& [name, exrName] : info.layers.data())
        for (const auto& ch : exrName.channels)
            DMX_LOG_TRACE("NLMFilter_test", "Layer name: ", name, ", Channel name: ", ch.name);

    DMXImage img{info.width, info.height, 1, LayerDictionary{aovs}};
    if (filenameBeauty != "")
        io->read(filenameBeauty, img, 0, AovDictionary{ {"beauty", aovs.at("beauty")} });
    else
        std::clog << "Beauty channel was not provided\n";

    if (filenameAlbedo != "")
        io->read(filenameAlbedo, img, 0, AovDictionary{ {"albedo", aovs.at("albedo")} });

    if (filenameNormal != "")
        io->read(filenameNormal, img, 0, AovDictionary{ {"normal", aovs.at("normal")} });

    if (filenameDepth != "")
        io->read(filenameDepth, img, 0, AovDictionary{ {"depth", aovs.at("depth")} });

    auto NLMFilter = FilterFactory::instance().create("NLMFilter");

    ParamDictionary params;
    params.addInt("radius", radius);
    params.addInt("patchRadius", patchRadius);
    params.addFloat("sigmaBeauty", sigmaBeauty);
    params.addFloat("sigmaAlbedo", sigmaAlbedo);
    params.addFloat("sigmaNormal", sigmaNormal);
    params.addFloat("sigmaDepth", sigmaDepth);
    params.addBackend("backend", backend);
    BackendResource res;
    res.threadPool = pool;
    params.addBackendResource("backendResource", res);
    EXPECT_NO_THROW(NLMFilter->setParams(params));

    EXPECT_NO_THROW(NLMFilter->apply(img, {"beauty", "albedo", "normal", "depth" }));

    // Write result to EXR
    std::vector<std::string> outLayers{};
    if (filterAllLayers) {
        for(const auto& [layer, chname] : aovs)
            outLayers.push_back(layer);
    } else {
        outLayers.push_back({"beauty"});
    }

    if (filenameBeauty != "")
        io->write(outputFileName+"_beauty.exr", img, {"beauty"} );
    else
        std::clog << "Beauty channel was not provided\n";

    if (filenameAlbedo != "")
        io->write(outputFileName+"_albedo.exr", img, {"albedo"} );

    if (filenameNormal != "")
        io->write(outputFileName+"_normal.exr", img, {"normal"} );

    if (filenameDepth != "")
        io->write(outputFileName+"_depth.exr", img, {"depth"} );
}

TEST_F(NLMFilterTest, ApplyNLMFilterKernelToTheImageRabbit)
{
    ThreadPool threadPool(0);
    std::string filename = "../examples/rabbit_pixel_art.exr";
    std::string outputFileName = "../tests/test_files/rabbit_pixel_art_nlm_7_3.exr";
    int radius = 7;
    int patchRadius = 3;
    float sigmaBeauty = 2.f;
    float sigmaAlbedo = 0.f;
    float sigmaNormal = 0.f;
    float sigmaDepth = 0.f;
    applyFilterToImageFile(filename, "", "", "", outputFileName, radius, patchRadius, 
                            sigmaBeauty, sigmaAlbedo, sigmaNormal, sigmaDepth, &threadPool, {{"beauty", "default"}});
}

TEST_F(NLMFilterTest, ApplyNLMFilterKernelToTheImagePalmAllLayers)
{
    ThreadPool threadPool(0);
    std::string filename = "../examples/palm_pixel_art_layered.exr";
    std::string outputFileName = "../tests/test_files/palm_pixel_art_layered_nlm_7_3_all_layers.exr";
    int radius = 7;
    int patchRadius = 3;
    float sigmaBeauty = 10.f;
    float sigmaAlbedo = 0.f;
    float sigmaNormal = 0.f;
    float sigmaDepth = 0.f;
    // applyFilterToImageFile(filename, "", "", "", outputFileName, radius, patchRadius, 
    //                         sigmaBeauty, sigmaAlbedo, sigmaNormal, sigmaDepth, &threadPool, 
    //                         { {"beauty", "rgba"}, {"layer1", "layer1"} }, Backend::CPU, true);
    std::unique_ptr<ImageIO> io = ImageIO::create(filename);
    auto info = io->getImageInfo(filename);
    for (const auto& [name, exrName] : info.layers.data())
        for (const auto& ch : exrName.channels)
            DMX_LOG_TRACE("NLMFilter_test", "Layer name: ", name, ", Channel name: ", ch.name);

    AovDictionary aovs = { 
        {"beauty", "rgba"},
        {"layer1", "layer1"}
    };

    dmxdenoiser::DMXImage img{info.width, info.height, 1, LayerDictionary{aovs}};
    DMX_LOG_TRACE("NLMFilter_test", "Created DMXImage: ", img.ToString());

    io->read(filename, img, 0, aovs);

    auto NLMFilter = FilterFactory::instance().create("NLMFilter");

    ParamDictionary params;
    params.addInt("radius", radius);
    params.addInt("patchRadius", patchRadius);
    params.addFloat("sigmaBeauty", sigmaBeauty);
    params.addFloat("sigmaAlbedo", sigmaAlbedo);
    params.addFloat("sigmaNormal", sigmaNormal);
    params.addFloat("sigmaDepth", sigmaDepth);
    params.addBackend("backend", Backend::CPU);
    BackendResource res;
    res.threadPool = &threadPool;
    params.addBackendResource("backendResource", res);
    EXPECT_NO_THROW(NLMFilter->setParams(params));
    EXPECT_NO_THROW(NLMFilter->apply(img));

    // Write result to EXR
    io->write(outputFileName, img, {"beauty", "layer1"});
}

#if DMX_ENABLE_HEAVY_TESTS
TEST_F(NLMFilterTest, ApplyNLMFilterKernelToTheImageForest)
{
    ThreadPool threadPool(0);
    std::string filenameBeauty = "../examples/sample_forest/TGB0203070_env_mid_anim_rgba.0001.exr";
    std::string filenameAlbedo = "../examples/sample_forest/TGB0203070_env_mid_anim_albedo.0001.exr";
    std::string filenameNormal = "../examples/sample_forest/TGB0203070_env_mid_anim_normal.0001.exr";
    std::string filenameDepth = "../examples/sample_forest/TGB0203070_env_mid_anim_depth.0001.exr";
    std::string outputFileName = "../tests/test_files/forest_nlm_2_1";
    int radius = 2;
    int patchRadius = 1;
    float sigmaBeauty = 10.0f;
    float sigmaAlbedo = 0.3f;
    float sigmaNormal = 0.45f;
    float sigmaDepth = 1.0f;
    applyFilterToImageFile(filenameBeauty, filenameAlbedo, filenameNormal, filenameDepth, 
                            outputFileName, radius, patchRadius, sigmaBeauty, sigmaAlbedo, 
                            sigmaNormal, sigmaDepth, &threadPool, 
                            {
                                {"beauty", "default"},
                                {"albedo", "default"},
                                {"normal", "default"},
                                {"depth", "depth"}
                            }, Backend::CPU, false);
}

TEST_F(NLMFilterTest, ApplyNLMFilterKernelToTheImageForestAllLayers)
{
    ThreadPool threadPool(0);
    std::string filenameBeauty = "../examples/sample_forest/TGB0203070_env_mid_anim_rgba.0001.exr";
    std::string filenameAlbedo = "../examples/sample_forest/TGB0203070_env_mid_anim_albedo.0001.exr";
    std::string filenameNormal = "../examples/sample_forest/TGB0203070_env_mid_anim_normal.0001.exr";
    std::string filenameDepth = "../examples/sample_forest/TGB0203070_env_mid_anim_depth.0001.exr";
    std::string outputFileName = "../tests/test_files/forest_nlm_2_1_all_layers.exr";
    int radius = 2;
    int patchRadius = 1;
    float sigmaBeauty = 10.0f;
    float sigmaAlbedo = 10.f;
    float sigmaNormal = 10.f;
    float sigmaDepth = 10.f;
    applyFilterToImageFile(filenameBeauty, filenameAlbedo, filenameNormal, filenameDepth, 
                            outputFileName, radius, patchRadius, sigmaBeauty, sigmaAlbedo, 
                            sigmaNormal, sigmaDepth, &threadPool, 
                            {
                                {"beauty", "default"},
                                {"albedo", "default"},
                                {"normal", "default"},
                                {"depth", "depth"}
                            },
                            Backend::CPU, true);
}
#endif
