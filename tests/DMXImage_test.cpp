#include <gtest/gtest.h>

#include <dmxdenoiser/DMXImage.hpp>

#include <iostream>
#include <vector>
#include <string>

using namespace dmxdenoiser;

TEST(DMXImage, InitializesCorrectlyWithValidParams)
{
    DMXImage img{};
    img.initialize(1, 2, 3, {"beauty", "albedo", "normal", "depth", "position"}, 4);

    EXPECT_EQ(img.data().size(), 120);
    EXPECT_EQ(img.getLayers().size(), 5);

    EXPECT_EQ(img.width(), 1);
    EXPECT_EQ(img.height(), 2);
    EXPECT_EQ(img.numFrames(), 3);
    EXPECT_EQ(img.numChannels(), 4);
}

TEST(DMXImage, ConstructorWorksCorrectlyWithValidParams)
{
    DMXImage img{1, 2, 3, {"beauty", "albedo", "normal", "depth", "position"}, 4};

    EXPECT_EQ(img.data().size(), 120);
    EXPECT_EQ(img.getLayers().size(), 5);

    EXPECT_EQ(img.width(), 1);
    EXPECT_EQ(img.height(), 2);
    EXPECT_EQ(img.numFrames(), 3);
    EXPECT_EQ(img.numChannels(), 4);
}

TEST(DMXImage, OptConstructorWorksCorrectlyWithValidParams)
{
    DMXImageOptions opt{1, 2, 3, {"beauty", "albedo", "normal", "depth", "position"}, 4};
    DMXImage img{opt};

    EXPECT_EQ(img.data().size(), 120);
    EXPECT_EQ(img.getLayers().size(), 5);

    EXPECT_EQ(img.width(), 1);
    EXPECT_EQ(img.height(), 2);
    EXPECT_EQ(img.numFrames(), 3);
    EXPECT_EQ(img.numChannels(), 4);
}

TEST(DMXImage, HasLayer)
{
    DMXImage img{1, 2, 3, {"beauty", "albedo", "normal", "depth", "position"}, 4};

    EXPECT_TRUE(img.hasLayer("albedo"));
    EXPECT_FALSE(img.hasLayer("unknown"));
}

TEST(DMXImage, hasValidLayerDictionary)
{
    DMXImage img{};
    EXPECT_FALSE(img.hasValidLayerDictionary());
    img.initialize(1, 2, 3, {"beauty", "albedo", "normal", "depth", "position"}, 4);
    EXPECT_TRUE(img.hasValidLayerDictionary());
}

TEST(DMXImage, GetLayerIndex)
{
    DMXImage img{1, 2, 3, {"beauty", "albedo", "normal", "depth", "position"}, 4};
    
    EXPECT_EQ(img.getLayerIndex("beauty"), 0);
    EXPECT_EQ(img.getLayerIndex("albedo"), 1);
    EXPECT_EQ(img.getLayerIndex("normal"), 2);
    EXPECT_EQ(img.getLayerIndex("depth"), 3);
    EXPECT_EQ(img.getLayerIndex("position"),4);
}

TEST(DMXImage, GetPixelIndex)
{
    DMXImage img{1, 2, 3, {"beauty", "albedo", "normal", "depth", "position"}, 4};
    
    EXPECT_EQ(img.getPixelIndex(0, 0, 0, "albedo"), 8);
    EXPECT_EQ(img.getPixelIndex(0, 0, 0, 1), 8);
    EXPECT_EQ(img.getPixelIndex(0, 1, 0, "albedo"), 12);
}

TEST(DMXImage, GetPixelIndexOutOfBounds)
{
    DMXImage img{1, 2, 3, {"beauty", "albedo", "normal", "depth", "position"}, 4};
    
    EXPECT_THROW(img.getPixelIndex(1, 0, 0, "albedo"), std::runtime_error);
    EXPECT_THROW(img.getPixelIndex(0, 3, 0, "albedo"), std::runtime_error);
    EXPECT_THROW(img.getPixelIndex(0, 0, 4, "albedo"), std::runtime_error);
    EXPECT_THROW(img.getPixelIndex(0, 0, 0, 10), std::runtime_error);
}

TEST(DMXImage, PixelAt)
{
    DMXImage img{2, 2, 2, {"beauty", "albedo", "normal"}, 4};
    
    for(int i = 0; i < static_cast<int>(img.data().size()); ++i)
    {
        img.data()[i] = static_cast<float>(i);
    }

    EXPECT_EQ(img.at(0,0,0,"albedo"), (PixelRGBA{16.0f, 17.0f, 18.0f, 19.0f}));
    EXPECT_EQ(img.at(0,0,0,1), (PixelRGBA{16.0f, 17.0f, 18.0f, 19.0f}));

    EXPECT_EQ(img.at(1,1,1,"normal"), (PixelRGBA{92.0f, 93.0f, 94.0f, 95.0f}));
    EXPECT_EQ(img.at(1,1,1,2), (PixelRGBA{92.0f, 93.0f, 94.0f, 95.0f}));
}

TEST(DMXImage, ToStringMethod)
{
    DMXImage img{2, 2, 2, {"beauty", "albedo", "normal"}, 4};
    std::string sRef{"DMXImage: \n    Dimensions: 2 x 2\n    Frames: 2\n    Channels: 4\n    Layers: beauty albedo normal "};
    std::cout << img.ToString() << '\n';
    EXPECT_EQ(img.ToString(), sRef);
}
