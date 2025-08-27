#include <gtest/gtest.h>

#include "TestConfig.hpp"
#include "AssertLogContains.hpp"
#include<dmxdenoiser/DMXImage.hpp>
#include<dmxdenoiser/ImageIO.hpp>
#include<dmxdenoiser/ImageIOExr.hpp>
#include<dmxdenoiser/Logger.hpp>

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

class ImageIOExrTest : public ::testing::Test {
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
                (char*) &rgbaPixels[0]+i*sizeof (rgbaPixels[0]) , // base
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

void compareTwoImages(const DMXImage& a, const DMXImage& b)
{
    std::size_t aSize = a.data().size();
    std::size_t bSize = b.data().size();
    ASSERT_EQ(aSize, bSize);

    auto aPixels = a.data();
    auto bPixels = b.data();
    for(int i = 0; i < aSize; ++i)
    {
        EXPECT_NEAR(aPixels[i], bPixels[i], 1e-6);
    }
}

TEST(ImageIOExr, WriteTestImage)
{
    ASSERT_NO_THROW(WriteEXRTestImage("../tests/test_files/test_exr_image2x2.exr"));
}

TEST(ImageIOExr, ReadTestImageInfo)
{
    std::map<std::string, std::vector<std::string>> layerMapRef = {
        {"default", {"R", "G", "B", "A"}},
        {"Layer", {"R", "G", "B", "A"}},
        {"Depth", {"z"}}
    };

    std::string filename = "../tests/test_files/test_exr_image2x2.exr";
    auto io = ImageIO::create(filename);
    auto imageInfo = io->getImageInfo(filename);

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

    std::cout << imageInfo.ToString() << '\n';
}

TEST(ImageIOExr, ReadWriteTestImageEXRFile)
{
    float rgbaPixelsRef[48] = {
        1.1f, 1.2f, 1.3f, 1.0f, 
        2.1f, 2.2f, 2.3f, 1.0f,
        3.1f, 3.2f, 3.3f, 1.0f, 
        4.1f, 4.2f, 4.3f, 1.0f,
        10.1f, 10.2f, 10.3f, 1.0f, 
        11.1f, 11.2f, 11.3f, 1.0f,
        12.1f, 12.2f, 12.3f, 1.0f, 
        13.1f, 13.2f, 13.3f, 1.0f,
        20.1f, 0.0f, 0.0f, 0.0f,
        21.1f, 0.0f, 0.0f, 0.0f,
        22.1f, 0.0f, 0.0f, 0.0f,
        23.1f, 0.0f, 0.0f, 0.0f
    };

    std::string pixelTypeRef[] = {
        "Half", "Half", "Half", "Half", "Float", "Half", "Half", "Half", "Half"
    };

    std::map<std::string, std::vector<std::string>> layerMapRef = {
        {"default", {"R", "G", "B", "A"}},
        {"Layer", {"R", "G", "B", "A"}},
        {"Depth", {"z"}}
    };
    
    std::string filename = "../tests/test_files/test_exr_image2x2.exr";

    AovDictionary aovs = {
        {"beauty", "default"},
        {"albedo", "Layer"},
        {"depth", "Depth"}
    };

    auto io = ImageIO::create(filename);
    ImageInfo info = io->getImageInfo(filename);
    
    DMXImage img{info.width, info.height, 1, LayerDictionary{aovs}};
    
    // Read EXR 
    io->read(filename, img, 0, aovs);
    
    for(int i = 0; i < img.data().size(); ++i)
    {
        EXPECT_NEAR(rgbaPixelsRef[i], img.data()[i], 1e-2);
    }

    for (const auto& [name, layerInfo] : img.getLayers().data())
        for (const auto& channel : layerInfo.channels)
        {
            EXPECT_NE(channel.pixelType, PixelType::Unknown);
        }
    
    // Write EXR 
    std::string outputFileName = "../tests/test_files/test_output_exr_image2x2.exr";
    io->write(outputFileName, img, {{"beauty"}, {"albedo"}, {"depth"}});

    // Read the new EXR
    info = io->getImageInfo(outputFileName);
    DMXImage img1{info.width, info.height, 1, LayerDictionary{aovs}};

    io->read(outputFileName, img1, 0, aovs);
    
    for(int i = 0; i < img1.data().size(); ++i)
    {
        EXPECT_NEAR(rgbaPixelsRef[i], img1.data()[i], 1e-2);
    }

    for (const auto& [name, layerInfo] : img1.getLayers().data())
        for (const auto& channel : layerInfo.channels)
        {
            EXPECT_NE(channel.pixelType, PixelType::Unknown);
        }

    compareTwoImages(img, img1);
}

TEST_F(ImageIOExrTest, ReadImageInfoEmptyPath)
{
    std::string filename = "";
    std::unique_ptr<ImageIO> io = ImageIOExr::create();
    EXPECT_THROW(io->getImageInfo(filename), std::runtime_error);
    assertLogContains(getLogPath(), "not found");
}

TEST_F(ImageIOExrTest, ReadImageEmptyPath)
{
    std::string filename = "";
    std::unique_ptr<ImageIO> io = ImageIOExr::create();
    DMXImage img;
    AovDictionary aovs = {
        {"beauty", "default"},
        {"albedo", "Layer"},
        {"depth", "Depth"}
    };
    EXPECT_THROW(io->read(filename, img, 0, aovs), std::runtime_error);
    assertLogContains(getLogPath(), "not found");
}
