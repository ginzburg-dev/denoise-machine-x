#include <gtest/gtest.h>

#include<dmxdenoiser/ImageIO.hpp>

#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <memory>

#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfPixelType.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfArray.h>
#include <Imath/half.h>
#include <Imath/ImathBox.h>

using namespace dmxdenoiser;

void WriteEXRTestImage(const std::string& fileName)
{
    half rgbaPixels[16] = {
        half(1.1f), half(1.2f), half(1.3f), half(1.0f), 
        half(2.1f), half(2.2f), half(2.3f), half(1.0f),
        half(3.1f), half(3.2f), half(3.3f), half(1.0f), 
        half(4.1f), half(4.2f), half(4.3f), half(1.0f)
    };

    half layerPixels[16] = {
        half(10.1f), half(10.2f), half(10.3f), half(1.0f), 
        half(11.1f), half(11.2f), half(11.3f), half(1.0f),
        half(12.1f), half(12.2f), half(12.3f), half(1.0f), 
        half(13.1f), half(13.2f), half(13.3f), half(1.0f)
    };

    float zPixels[4] = {
        20.1f, 21.1f,
        22.1f, 23.1f
    };

    int width = 2;
    int height = 2;
    Imf::Header header (width, height);
    Imath::Box2i dataWindow{Imath::V2i(0, 0), Imath::V2i(width - 1, height - 1)};
    header.dataWindow() = dataWindow;
    header.channels ().insert ("R", Imf::Channel (Imf::HALF));
    header.channels ().insert ("G", Imf::Channel (Imf::HALF));
    header.channels ().insert ("B", Imf::Channel (Imf::HALF));
    header.channels ().insert ("A", Imf::Channel (Imf::HALF));

    header.channels ().insert ("Layer.R", Imf::Channel (Imf::HALF));
    header.channels ().insert ("Layer.G", Imf::Channel (Imf::HALF));
    header.channels ().insert ("Layer.B", Imf::Channel (Imf::HALF));
    header.channels ().insert ("Layer.A", Imf::Channel (Imf::HALF));

    header.channels ().insert ("Depth.z", Imf::Channel (Imf::FLOAT));

    Imf::OutputFile file (fileName.c_str(), header);

    Imf::FrameBuffer frameBuffer;

    std::string channels = "RGBA";
    for (int i = 0; i < 4; ++i)
    {
        std::string ch = "";
        ch += channels[i];
        frameBuffer.insert (
            ch, // name
            Imf::Slice (
                Imf::HALF,                        // type
                (char*) &rgbaPixels[i],             // base
                sizeof (rgbaPixels[0]) * 4,       // xStride
                sizeof (rgbaPixels[0]) * width * 4)); // yStride
    }

    for (int i = 0; i < 4; ++i)
    {
        std::string ch = "Layer.";
        ch += channels[i];
        frameBuffer.insert (
        ch, // name
        Imf::Slice (
            Imf::HALF,                        // type
            (char*) &layerPixels[i],             // base
            sizeof (layerPixels[0]) * 4,       // xStride
            sizeof (layerPixels[0]) * width * 4)); // yStride
    }

    frameBuffer.insert (
        "Depth.z", // name
        Imf::Slice (
            Imf::FLOAT,                        // type
            (char*) zPixels,             // base
            sizeof (zPixels[0]) * 1,       // xStride
            sizeof (zPixels[0]) * width * 1)); // yStride

    file.setFrameBuffer (frameBuffer);
    file.writePixels (dataWindow.max.y - dataWindow.min.y + 1);

}

TEST(ImageIO, WriteTestImage)
{
    ASSERT_NO_THROW(WriteEXRTestImage("../tests/test_files/test_exr_image2x2.exr"));
}

TEST(ImageIO, ReadTestImageInfo)
{
    std::map<std::string, std::vector<std::string>> layerMapRef = {
        {"default", {"R", "G", "B", "A"}},
        {"Layer", {"R", "G", "B", "A"}},
        {"Depth", {"z"}}
    };

    std::unique_ptr<ImageIO> IO = std::make_unique<ExrImageIO>();
    auto imageInfo = IO->getImageInfo("../tests/test_files/test_exr_image2x2.exr");

    EXPECT_EQ(imageInfo.width, 2);
    EXPECT_EQ(imageInfo.height, 2);

    EXPECT_EQ(imageInfo.type, ImageFileType::EXR);

    ASSERT_TRUE(imageInfo.params.getSingleParam<int>("compression"));
    EXPECT_EQ(imageInfo.params.getSingleParam<int>("compression"), 3);

    for(const auto& [lname, layerInfo] : imageInfo.layers.data())
    {
        auto itLayer = layerMapRef.find(lname);
        ASSERT_TRUE(itLayer != layerMapRef.end());

        auto channels = itLayer->second;
        for (int i = 0; i < layerInfo.channels.size(); ++i)
        {
            EXPECT_EQ(channels[i], layerInfo.channels[i].name );
        }
    }
}
