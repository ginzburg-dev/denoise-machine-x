#include <gtest/gtest.h>

#include "TestConfig.hpp"
#include "AssertLogContains.hpp"
#include<dmxdenoiser/DMXImage.hpp>
#include<dmxdenoiser/ImageIO.hpp>
#include<dmxdenoiser/Logger.hpp>

#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <memory>

#include <Imath/half.h>

using namespace dmxdenoiser;

class ImageIOTest : public ::testing::Test {
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

TEST(ImageIO, CopyChannelBufferToDMXImageAndBack)
{
    float dataRef[32] = {
        0, 4, 8, 12, 1, 5, 9, 
        13, 2, 6, 10, 14, 3, 7, 
        11, 15, 16, 20, 24, 28, 
        17, 21, 25, 29, 18, 22, 
        26, 30, 19, 23, 27, 31};

    DMXImageOptions opt;
    opt.width = 2;
    opt.height = 2;
    opt.numFrames = 1;
    opt.layers = LayerDictionary{AovDictionary{{"beauty", "default"}, {"albedo", "albedo1"}}};
    DMXImage img{opt};

    std::vector<ChannelBuffer> buffersBeauty{};
    buffersBeauty.emplace_back("R", PixelType::Half, 4);
    buffersBeauty.emplace_back("G", PixelType::Half, 4);
    buffersBeauty.emplace_back("B", PixelType::Half, 4);
    buffersBeauty.emplace_back("A", PixelType::Half, 4);
    img.getLayers().getLayer("beauty")->channels.emplace_back("R", PixelType::Half);
    img.getLayers().getLayer("beauty")->channels.emplace_back("G", PixelType::Half);
    img.getLayers().getLayer("beauty")->channels.emplace_back("B", PixelType::Half);
    img.getLayers().getLayer("beauty")->channels.emplace_back("A", PixelType::Half);

    std::vector<ChannelBuffer> buffersAlbedo{};
    buffersAlbedo.emplace_back("R", PixelType::UInt32, 4);
    buffersAlbedo.emplace_back("G", PixelType::UInt32, 4);
    buffersAlbedo.emplace_back("B", PixelType::UInt32, 4);
    buffersAlbedo.emplace_back("A", PixelType::UInt32, 4);
    img.getLayers().getLayer("albedo")->channels.emplace_back("R", PixelType::UInt32);
    img.getLayers().getLayer("albedo")->channels.emplace_back("G", PixelType::UInt32);
    img.getLayers().getLayer("albedo")->channels.emplace_back("B", PixelType::UInt32);
    img.getLayers().getLayer("albedo")->channels.emplace_back("A", PixelType::UInt32);
    
    int count = 0;
    for(auto& buffer : buffersBeauty)
    {
        for(auto& p : buffer.data().halfs)
        {
            p = half(count++);
        }
    }
    for(auto& buffer : buffersAlbedo)
    {
        for(auto& p : buffer.data().uint32s)
        {
            p = count++;
        }
    }
    
    copyChannelBuffersToDMXImage(buffersBeauty, "beauty", 0, img);
    copyChannelBuffersToDMXImage(buffersAlbedo, "albedo", 0, img);
    
    for(int i = 0; i < img.data().size(); ++i)
        EXPECT_EQ(dataRef[i], img.data()[i]);
    
    auto buff = copyDMXImageToChannelBuffers(img,{{"beauty"},{"albedo"}});

    count = 0;
    for(int i = 0; i < 4; ++i)
        for(const auto& ch : buff[i].data().halfs)
        {
            EXPECT_EQ(count, ch);
            ++count;
        }

    for(int i = 4; i < 8; ++i)
        for(const auto& ch : buff[i].data().uint32s)
        {
            EXPECT_EQ(count, ch);
            ++count;
        }
}

TEST_F(ImageIOTest, CreateImageIOEmptyPath)
{
    std::string filename = "";
    std::unique_ptr<ImageIO> io;
    EXPECT_THROW(ImageIO::create(filename), std::runtime_error);
    assertLogContains(getLogPath(), "not found");
}
