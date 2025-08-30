#include <gtest/gtest.h>

#include <dmxdenoiser/utils/NumericUtils.hpp>

#include <limits>

using namespace dmxdenoiser;

TEST(NumericUtils, FloatsEqualEpsilonBoundary)
{
    float a = 1.000001f;
    float b = a + 1e-9f;
    float c = a + 1e-5f;
    EXPECT_TRUE(floatsEqual(a, b));
    EXPECT_FALSE(floatsEqual(a, c));
}

TEST(NumericUtils, FloatsBlend)
{
    float a = 1.0f;
    float b = 2.0f;
    float eps = std::numeric_limits<float>::epsilon();
    EXPECT_NEAR(floatsBlend(a, b, 0.0f), a, eps); // expects a
    EXPECT_NEAR(floatsBlend(a, b, 1.0f), b, eps); // expects b
    EXPECT_NEAR(floatsBlend(a, b, 0.5f), 1.5f, eps);
    
    EXPECT_NEAR(floatsBlend(a, b, -100.0f), a, eps); // clamping t test, expects a
    EXPECT_NEAR(floatsBlend(a, b, 100.0f), b, eps); // clamping t test, expects b
}
