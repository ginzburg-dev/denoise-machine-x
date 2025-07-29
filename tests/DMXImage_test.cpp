#include <gtest/gtest.h>

#include <dmxdenoiser/DMXImage.hpp>

#include <vector>
#include <string>

using namespace dmxdenoiser;

TEST(DMXImage, InitializeMethod)
{
    DMXImage img{};
    img.initialize(1, 2, 3, {"beauty", "albedo", "normal", "depth", "position"}, 4);
    EXPECT_EQ(img.data().size(), 120);
    EXPECT_EQ(img.getLayers().size(), 5);
    EXPECT_EQ(img.width(), 1);
    EXPECT_EQ(img.height(), 2);
    EXPECT_EQ(img.numFrames(), 3);
    EXPECT_EQ(img.numChannels(), 4);
    EXPECT_TRUE(img.hasLayer("albedo"));
    EXPECT_FALSE(img.hasLayer("unknown"));
    EXPECT_EQ(img.getLayerIndex("beauty"), 0);
    EXPECT_EQ(img.getLayerIndex("albedo"), 1);
    EXPECT_EQ(img.getLayerIndex("normal"), 2);
    EXPECT_EQ(img.getLayerIndex("depth"), 3);
    EXPECT_EQ(img.getLayerIndex("position"),4);
    //EXPECT_EQ(img.at(0, 0, 0, "albedo")), PixelRGBA());
}
