#include <gtest/gtest.h>

#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/filters/ConvolutionFilter.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/ImageIOExr.hpp>
#include <dmxdenoiser/ImageIOFactory.hpp>

#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <memory>

using namespace dmxdenoiser;

void applyFilterToImageFile(
    const std::string& filename, 
    const std::string& outputFileName,
    const Kernel2D& kernel)
{
    AovDictionary aovs = { {"beauty", "default"} };
    std::unique_ptr<ImageIO> io = createImageIO(filename);
    auto info = io->getImageInfo(filename);

    DMXImage img{info.width, info.height, 1, LayerDictionary{aovs}};

    io->read(filename, img, 0, aovs);

    auto convoFilter = FilterFactory::instance().create("ConvolutionFilter");

    ParamDictionary params;
    params.addKernel2D("kernel", kernel);

    EXPECT_NO_THROW(convoFilter->setParams(params));

    EXPECT_NO_THROW(convoFilter->apply(img));

    // Write result to EXR
    io->write(outputFileName, img, {{"beauty"}});
}

TEST(ConvolutionFilter, NoKernelParameterSet)
{
    
}

TEST(ConvolutionFilter, SimpleConvolveAverages)
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

TEST(ConvolutionFilter, SetFilteringLayersAndFrames)
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

TEST(ConvolutionFilter, SimpleGaussianKernel)
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

TEST(ConvolutionFilter, ApplyBoxFilterKernelToTheImage)
{
    std::string filename = "../examples/rabbit_pixel_art.exr";
    std::string outputFileName = "../tests/test_files/rabbit_pixel_art_convo_box_3x3.exr";
    auto boxKernel = FilterKernels::getBoxKernel(3);
    applyFilterToImageFile(filename, outputFileName, boxKernel);
}

TEST(ConvolutionFilter, ApplyGaussianFilterKernelToTheImage)
{
    std::string filename = "../examples/rabbit_pixel_art.exr";
    std::string outputFileName = "../tests/test_files/rabbit_pixel_art_convo_gaussan_sigma2_3x3.exr";
    float sigma = 2.0f;
    auto gaussianKernel = FilterKernels::getGaussianKernel(3, sigma);
    applyFilterToImageFile(filename, outputFileName, gaussianKernel);
}

TEST(ConvolutionFilter, ApplySobelFilterKernelToTheImage)
{
    std::string filename = "../examples/rabbit_pixel_art.exr";
    
    auto kernelX = FilterKernels::getSobelKernelX();
    auto kernelY = FilterKernels::getSobelKernelY();

    applyFilterToImageFile(filename, "../tests/test_files/rabbit_pixel_art_convo_sobel_x_3x3.exr", kernelX);
    applyFilterToImageFile(filename, "../tests/test_files/rabbit_pixel_art_convo_sobel_y_3x3.exr", kernelY);
}
