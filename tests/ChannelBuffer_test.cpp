#include <gtest/gtest.h>

#include <dmxdenoiser/ChannelBuffer.hpp>

#include <vector>

using namespace dmxdenoiser;

TEST(ChannelBuffer, Allocate)
{
    ChannelBuffer halfBuf("Half",PixelType::Half, 10);
    ASSERT_EQ(halfBuf.data().halfs.size(), 10);
    ChannelBuffer doubleBuf("Double",PixelType::Double, 20);
    ASSERT_EQ(doubleBuf.data().doubles.size(), 20);
    ChannelBuffer floatBuf("Float",PixelType::Float, 30);
    ASSERT_EQ(floatBuf.data().floats.size(), 30);
    ChannelBuffer uint8Buf("Uint8",PixelType::UInt8, 40);
    ASSERT_EQ(uint8Buf.data().uint8s.size(), 40);
    ChannelBuffer uint16Buf("Uint16",PixelType::UInt16, 50);
    ASSERT_EQ(uint16Buf.data().uint16s.size(), 50);
    ChannelBuffer uint32Buf("Uint32",PixelType::UInt32, 60);
    ASSERT_EQ(uint32Buf.data().uint32s.size(), 60);
}

TEST(ChannelBuffer, ReadAndWriteHalfBuffer)
{
    ChannelBuffer halfBuf("Half", PixelType::Half, 10);
    char* ptr = halfBuf.getRawPtr();
    for (int i = 0; i < 10; ++i)
        *reinterpret_cast<half*>(ptr + sizeof(half)*i) = half(2.5); 
    for (int i = 0; i < 10; ++i)
        EXPECT_NEAR(*(reinterpret_cast<half*>(ptr)+i), half(2.5),1e-6);
}

TEST(ChannelBuffer, LayerChannelBufferMapSet)
{
    LayerChannelBufferMap bufMap;
    bufMap.set("beauty", ChannelBuffer{"R",PixelType::Half, 100});
    bufMap.set("beauty", ChannelBuffer{"G",PixelType::Half, 100});
    bufMap.set("beauty", ChannelBuffer{"B",PixelType::Half, 100});
    bufMap.set("beauty", "A", PixelType::Half, 100);

    ASSERT_TRUE(bufMap.hasLayer("beauty"));
    ASSERT_TRUE(bufMap.hasBuffer("beauty", "R"));

    ASSERT_TRUE(bufMap.getLayerBuffers("beauty"));
    EXPECT_EQ(bufMap.getLayerBuffers("beauty")->size(), 4);

    ASSERT_TRUE(bufMap.getBuffer("beauty", "R"));
    ASSERT_TRUE(bufMap.getBuffer("beauty", "G"));
    ASSERT_TRUE(bufMap.getBuffer("beauty", "B"));
    ASSERT_TRUE(bufMap.getBuffer("beauty", "A"));
    
    EXPECT_EQ(bufMap.getBuffer("beauty", "R")->data().halfs.size(), 100);
}
