#include <gtest/gtest.h>

#include <dmxdenoiser/utils/FileUtils.hpp>

#include <string>

using namespace dmxdenoiser;

TEST(FileUtils, FormatSequencePath)
{
    EXPECT_EQ(formatSequencePath("./file.####.exr",1),"./file.0001.exr");
    EXPECT_EQ(formatSequencePath("./file.####.exr",-1),"./file.-0001.exr");
    EXPECT_EQ(formatSequencePath("./file.###.exr",100),"./file.100.exr");
    EXPECT_EQ(formatSequencePath("./file.###.exr",-100),"./file.-100.exr");
}
