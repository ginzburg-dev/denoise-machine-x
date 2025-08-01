#include <gtest/gtest.h>

#include<dmxdenoiser/DMXImage.hpp>
#include<dmxdenoiser/FilterKernels.hpp>
#include<dmxdenoiser/FilterFactory.hpp>
#include<dmxdenoiser/filters/ConvolutionFilter.hpp>
#include<dmxdenoiser/ParamDictionary.hpp>
#include<dmxdenoiser/ImageIO.hpp>
#include<dmxdenoiser/ImageIOFactory.hpp>

#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <memory>

using namespace dmxdenoiser;

TEST(ConvolutionFilter, ApplyBoxFilterKernelToTheImage)
{
    AovDictionary aovs = {
        {"beauty", "default"}
    };

    std::string filename = "../examples/rabbit_pixel_art.exr";
    std::unique_ptr<ImageIO> io = createImageIO(filename);
    auto info = io->getImageInfo(filename);
    DMXImage img{info.width, info.height, 1, LayerDictionary{aovs}};

    io->read(filename, img, 0, aovs);

    auto convoFilter = FilterFactory::instance().create("ConvolutionFilter");

    auto kernel = FilterKernels::getBoxKernel(3);

    ParamDictionary params;
    params.addFloatArray("kernel", kernel.m_data);
    params.addFloat("strength", 1.0f);
    params.addBool("filterAlpha", false);
    params.addIntArray("frames", {0});
    params.addStringArray("layers", {"beauty"});
    EXPECT_NO_THROW(convoFilter->setParams(params));

    EXPECT_NO_THROW(convoFilter->apply(img));

    // Write result to EXR
    std::string outputFileName = "../tests/test_files/rabbit_pixel_art_convo_box_3x3.exr";
    io->write(outputFileName, img, {{"beauty"}});
}

TEST(ConvolutionFilter, ApplySobelFilterKernelToTheImage)
{
    AovDictionary aovs = {
        {"beauty", "default"}
    };

    std::string filename = "../examples/rabbit_pixel_art.exr";
    std::unique_ptr<ImageIO> io = createImageIO(filename);
    auto info = io->getImageInfo(filename);
    DMXImage img{info.width, info.height, 1, LayerDictionary{aovs}};

    io->read(filename, img, 0, aovs);

    auto convoFilter = FilterFactory::instance().create("ConvolutionFilter");

    auto kernel = FilterKernels::getSobelKernelX();

    ParamDictionary params;
    params.addFloatArray("kernel", kernel.m_data);
    params.addFloat("strength", 1.0f);
    params.addBool("filterAlpha", false);
    params.addIntArray("frames", {0});
    params.addStringArray("layers", {"beauty"});
    EXPECT_NO_THROW(convoFilter->setParams(params));

    EXPECT_NO_THROW(convoFilter->apply(img));

    // Write result to EXR
    std::string outputFileName = "../tests/test_files/rabbit_pixel_art_convo_sobel_x_3x3.exr";
    io->write(outputFileName, img, {{"beauty"}});
}
