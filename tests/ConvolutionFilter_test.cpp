#include <gtest/gtest.h>

#include "TestConfig.hpp"
#include "AssertLogContains.hpp"
#include <dmxdenoiser/Backend.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/filters/ConvolutionFilter.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
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

class ConvolutionFilterTest : public ::testing::Test {
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
    const std::string& filename, 
    const std::string& outputFileName,
    const Kernel2D& kernel,
    ThreadPool* pool = nullptr,
    Backend backend=Backend::CPU)
{
    AovDictionary aovs = { {"beauty", "default"} };
    std::unique_ptr<ImageIO> io = ImageIO::create(filename);
    auto info = io->getImageInfo(filename);

    DMXImage img{info.width, info.height, 1, LayerDictionary{aovs}};

    io->read(filename, img, 0, aovs);

    auto convoFilter = FilterFactory::instance().create("ConvolutionFilter");

    ParamDictionary params;
    params.addKernel2D("kernel", kernel);
    params.addBackend("backend", backend);
    BackendResource res;
    res.threadPool = pool;
    params.addBackendResource("backendResource", res);
    EXPECT_NO_THROW(convoFilter->setParams(params));

    EXPECT_NO_THROW(convoFilter->apply(img));

    // Write result to EXR
    io->write(outputFileName, img, {{"beauty"}});
}

TEST_F(ConvolutionFilterTest, ParametersNotSet)
{
    ParamDictionary params;
    //params.addKernel2D("kernel", kernel);
    //params.addBackend("backend", backend);
    auto convoFilter = DMX_CREATE_FILTER("ConvolutionFilter");
    DMXImage img{};
    EXPECT_THROW(convoFilter->apply(img), std::runtime_error);

    // Check log
    std::string tag{"ConvolutionFilter"};
    std::string msg{"Kernel is empty, size=0x0"};
    assertLogContains(getLogPath(), "ERROR", tag, msg);
}

TEST_F(ConvolutionFilterTest, ParametersNotSetInfoLog)
{
    ParamDictionary params;
    params.addKernel2D("kernel", FilterKernels::getBoxKernel(3));
    //params.addBackend("backend", backend);
    auto convoFilter = DMX_CREATE_FILTER("ConvolutionFilter");
    DMXImage img(10, 10, 1, LayerDictionary{"beauty"});
    EXPECT_NO_THROW(convoFilter->setParams(params));
    EXPECT_NO_THROW(convoFilter->apply(img));

    // Check log
    assertLogContains(getLogPath(), "'strength'", "'strength'",
        "'layers'", "'filterAlpha'", "'backend'", "'backendResource'");
}

TEST_F(ConvolutionFilterTest, ParametersSetFramesLayersInfoLog)
{
    ParamDictionary params;
    params.addKernel2D("kernel", FilterKernels::getBoxKernel(3));
    params.addStringArray("layers", {"beauty", "diffuse", "specular", "unknown"});
    params.addIntArray("frames", {1, 2, 3, 4, 5, 6, 7, 8});
    //params.addBackend("backend", backend);
    auto convoFilter = DMX_CREATE_FILTER("ConvolutionFilter");
    DMXImage img(10, 10, 5, LayerDictionary{"beauty", "diffuse", "specular", "normal", "depth"});
    EXPECT_NO_THROW(convoFilter->setParams(params));
    EXPECT_NO_THROW(convoFilter->apply(img));

    // Check log
    assertLogContains(getLogPath(), "requested frame", "requested layer", "not found", "6", "7", "8", "unknown");
}


TEST_F(ConvolutionFilterTest, SimpleConvolveAverages)
{
    std::size_t imageSize = 3;
    AovDictionary aovs = { {"beauty", "default"} };
    DMXImage img(imageSize, imageSize, 1, LayerDictionary{aovs});
    ConvolutionFilter filter;

    // Fill the pixel data
    /*
    for (int y = 0; y < imageSize; ++y)
        for (int x = 0; x < imageSize; ++x)
        {
            img.data()[(y*size + x)*img.numChannels() + 0] = 1.0f; // R
            img.data()[(y*size + x)*img.numChannels() + 1] = 2.0f; // G
            img.data()[(y*size + x)*img.numChannels() + 2] = 3.0f; // B
            img.data()[(y*size + x)*img.numChannels() + 3] = 4.0f; // A
        }
    */    

    Kernel2D boxKernel = FilterKernels::getBoxKernel(3);
    ParamDictionary params;
    params.addKernel2D("kernel", boxKernel);
    filter.setParams(params);

    filter.apply(img);

    /*
    for (int y = 0; y < imageSize; ++y)
        for (int x = 0; x < imageSize; ++x)
        {
            auto p = img.at(x, y, 0, 0);
            EXPECT_FLOAT_EQ(p.r, 1.0f);
            EXPECT_FLOAT_EQ(p.g, 2.0f);
            EXPECT_FLOAT_EQ(p.b, 3.0f);
            EXPECT_FLOAT_EQ(p.a, 4.0f);
        }
    */
}

TEST_F(ConvolutionFilterTest, SetFilteringLayersAndFrames)
{
    std::size_t size = 3;
    AovDictionary aovs = { {"beauty", "default"} };
    DMXImage img(size, size, 1, LayerDictionary{aovs});
    ConvolutionFilter filter;

    // Fill the pixel data
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x)
        {
            img.data()[(y*size + x)*img.numChannels() + 0] = 1.0f; // R
            img.data()[(y*size + x)*img.numChannels() + 1] = 2.0f; // G
            img.data()[(y*size + x)*img.numChannels() + 2] = 3.0f; // B
            img.data()[(y*size + x)*img.numChannels() + 3] = 4.0f; // A
        }
            

    Kernel2D boxKernel = FilterKernels::getBoxKernel(3);
    ParamDictionary params;
    params.addKernel2D("kernel", boxKernel);
    filter.setParams(params);

    filter.apply(img);

    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x)
        {
            auto p = img.at(x, y, 0, 0);
            EXPECT_FLOAT_EQ(p.r, 1.0f);
            EXPECT_FLOAT_EQ(p.g, 2.0f);
            EXPECT_FLOAT_EQ(p.b, 3.0f);
            EXPECT_FLOAT_EQ(p.a, 4.0f);
        }
}

TEST_F(ConvolutionFilterTest, SimpleGaussianKernel)
{
    std::size_t size = 3;
    AovDictionary aovs = { {"beauty", "default"} };
    DMXImage img(size, size, 1, LayerDictionary{aovs});
    ConvolutionFilter filter;

    // Fill the pixel data
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x)
        {
            img.data()[(y*size + x)*img.numChannels() + 0] = 1.0f; // R
            img.data()[(y*size + x)*img.numChannels() + 1] = 2.0f; // G
            img.data()[(y*size + x)*img.numChannels() + 2] = 3.0f; // B
            img.data()[(y*size + x)*img.numChannels() + 3] = 4.0f; // A
        }
            

    Kernel2D gaussianKernel = FilterKernels::getGaussianKernel(3, 1);
    std::cout << gaussianKernel.ToString() << '\n';
    ParamDictionary params;
    params.addKernel2D("kernel", gaussianKernel);
    filter.setParams(params);

    filter.apply(img);

    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x)
        {
            auto p = img.at(x, y, 0, 0);
            EXPECT_FLOAT_EQ(p.r, 1.0f);
            EXPECT_FLOAT_EQ(p.g, 2.0f);
            EXPECT_FLOAT_EQ(p.b, 3.0f);
            EXPECT_FLOAT_EQ(p.a, 4.0f);
        }
}

TEST_F(ConvolutionFilterTest, ApplyBoxFilterKernelToTheImage)
{
    std::string filename = "../examples/rabbit_pixel_art.exr";
    std::string outputFileName = "../tests/test_files/rabbit_pixel_art_convo_box_3x3.exr";
    auto boxKernel = FilterKernels::getBoxKernel(3);
    applyFilterToImageFile(filename, outputFileName, boxKernel);
}

TEST_F(ConvolutionFilterTest, ApplyGaussianFilterKernelToTheImage)
{
    std::string filename = "../examples/rabbit_pixel_art.exr";
    std::string outputFileName = "../tests/test_files/rabbit_pixel_art_convo_gaussan_sigma2_3x3.exr";
    float sigma = 2.0f;
    auto gaussianKernel = FilterKernels::getGaussianKernel(3, sigma);
    applyFilterToImageFile(filename, outputFileName, gaussianKernel);
}

TEST_F(ConvolutionFilterTest, ApplySobelFilterKernelToTheImage)
{
    std::string filename = "../examples/rabbit_pixel_art.exr";
    
    auto kernelX = FilterKernels::getSobelKernelX();
    auto kernelY = FilterKernels::getSobelKernelY();

    applyFilterToImageFile(filename, "../tests/test_files/rabbit_pixel_art_convo_sobel_x_3x3.exr", kernelX);
    applyFilterToImageFile(filename, "../tests/test_files/rabbit_pixel_art_convo_sobel_y_3x3.exr", kernelY);
}

TEST_F(ConvolutionFilterTest, ApplyGaussianFilterKernelToTheImageParallel)
{
    ThreadPool threadPool(0);

    std::string filename = "../examples/rabbit_pixel_art.exr";
    std::string outputFileName = "../tests/test_files/rabbit_pixel_art_parallel_convo_gaussan_sigma2_3x3.exr";
    float sigma = 2.0f;
    auto gaussianKernel = FilterKernels::getGaussianKernel(3, sigma);
    applyFilterToImageFile(filename, outputFileName, gaussianKernel, &threadPool);
}

TEST_F(ConvolutionFilterTest, ApplyGaussianFilterKernelToTheImageParallelSingleThread)
{
    std::string filename = "../examples/rabbit_pixel_art.exr";
    std::string outputFileName = "../tests/test_files/rabbit_pixel_art_parallel_single_convo_gaussan_sigma2_3x3.exr";
    float sigma = 2.0f;
    auto gaussianKernel = FilterKernels::getGaussianKernel(3, sigma);
    applyFilterToImageFile(filename, outputFileName, gaussianKernel);
}
