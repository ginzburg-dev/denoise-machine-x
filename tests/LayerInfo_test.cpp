#include <gtest/gtest.h>

#include <dmxdenoiser/LayerInfo.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace dmxdenoiser;

TEST(LayerInfo, CreateLayerInfoAndOrderingChannels)
{
    std::vector<std::string> channelsRef{"Red", "Green", "Blue", "A", "Z", "Depth"};
    LayerInfo info{"beauty"};
    info.addChannel("A");
    info.addChannel("Blue");
    info.addChannel("Green");
    info.addChannel("Red");
    info.addChannel("Z");
    info.addChannel("Depth");

    EXPECT_EQ(info.originalName, "beauty");
    for(int i = 0; i < channelsRef.size(); ++i)
        EXPECT_EQ(info.channels[i].name, channelsRef[i]);
}

TEST(LayerInfo, GetLayer)
{
    LayerInfo info{"beauty"};
    info.addChannel("A", PixelType::Float);
    auto channelPtr = info.getChannel("A");
    ASSERT_TRUE(channelPtr);
    EXPECT_EQ(channelPtr->name, "A");
    EXPECT_EQ(channelPtr->pixelType, PixelType::Float);
}

TEST(LayerInfo, CustomChannelSort)
{
    std::string CUSTOM_CHANNEL_ORDER = "zrgbad";
    std::vector<std::string> channelsRef{"Z", "Red", "Green", "Blue", "A", "Depth"};

    LayerInfo info{"beauty"};
    info.addChannel("A");
    info.addChannel("Blue");
    info.addChannel("Green");
    info.addChannel("Red");
    info.addChannel("Z");
    info.addChannel("Depth");

    info.sortChannels(CUSTOM_CHANNEL_ORDER);

    EXPECT_EQ(info.originalName, "beauty");
    for(int i = 0; i < channelsRef.size(); ++i)
        EXPECT_EQ(info.channels[i].name, channelsRef[i]);
}
