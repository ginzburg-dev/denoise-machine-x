#include <gtest/gtest.h>

#include <dmxdenoiser/util/FloatsEqual.hpp>

using namespace dmxdenoiser;

TEST(FloatsEqual, EpsilonBoundary)
{
    float a = 1.000001f;
    float b = a + 1e-9f;
    float c = a + 1e-5f;
    EXPECT_TRUE(floatsEqual(a, b));
    EXPECT_FALSE(floatsEqual(a, c));
}
