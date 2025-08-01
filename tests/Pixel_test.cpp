#include <gtest/gtest.h>

#include <dmxdenoiser/Pixel.hpp>
#include <dmxdenoiser/util/FloatsEqual.hpp>

#include <limits>

constexpr float Inf = std::numeric_limits<float>::infinity();
constexpr float Nan = std::numeric_limits<float>::quiet_NaN();

using namespace dmxdenoiser;

TEST(Pixel, PixelRGBAComparison)
{
    PixelRGBA p1 = {1.2f, 3.4f, 5.6f, 1.0f};
    PixelRGBA p2 = {1.2f, 3.4f, 5.6f, 1.0f};
    PixelRGBA p3 = {0.0f, 0.0f, 0.0f, 1.0f};
    EXPECT_TRUE(p1 == p2);
    EXPECT_TRUE(p1 != p3);
}

TEST(Pixel, PixelRGBAViewComparison)
{
    PixelRGBA p1 = {1.2f, 3.4f, 5.6f, 1.0f};
    PixelRGBA p2 = {1.2f, 3.4f, 5.6f, 1.0f};
    PixelRGBA p3 = {0.0f, 0.0f, 0.0f, 1.0f};

    PixelRGBAView pv1 = p1;
    PixelRGBAView pv2 = p2;
    PixelRGBAView pv3 = p3;
    

    EXPECT_TRUE(pv1 == pv2);
    EXPECT_TRUE(pv1 != pv3);
}

TEST(Pixel, PixelRGBAViewAssigning)
{
    PixelRGBA p1 = {1.2f, 3.4f, 5.6f, 1.0f};
    PixelRGBA p2 = {0.0f, 0.0f, 0.0f, 1.0f};

    PixelRGBAView pv1 = p1;
    PixelRGBAView pv2 = p2;

    pv2 = pv1; // assign View and View
    EXPECT_TRUE(floatsEqual(pv2.r, 1.2f));
    EXPECT_TRUE(floatsEqual(pv2.g, 3.4f));
    EXPECT_TRUE(floatsEqual(pv2.b, 5.6f));
    EXPECT_TRUE(floatsEqual(pv2.a, 1.0f));
}

TEST(Pixel, PixelRGBAViewAssigningPixelRGBA)
{
    PixelRGBA p1 = {1.2f, 3.4f, 5.6f, 1.0f};
    PixelRGBA p2 = {0.0f, 0.0f, 0.0f, 1.0f};

    PixelRGBAView pv1 = p2;

    pv1 = p1; // assign PixelRGBA
    EXPECT_TRUE(floatsEqual(pv1.r, 1.2f));
    EXPECT_TRUE(floatsEqual(pv1.g, 3.4f));
    EXPECT_TRUE(floatsEqual(pv1.b, 5.6f));
    EXPECT_TRUE(floatsEqual(pv1.a, 1.0f));
}

TEST(Pixel, PixelRGBAAssigningPixelRGBAView)
{
    PixelRGBA p1 = {1.2f, 3.4f, 5.6f, 1.0f};
    PixelRGBA p2 = {0.0f, 0.0f, 0.0f, 1.0f};

    PixelRGBAView pv1 = p2;

    p1 = pv1; // assign PixelRGBAView
    EXPECT_TRUE(floatsEqual(p1.r, 0.0f));
    EXPECT_TRUE(floatsEqual(p1.g, 0.0f));
    EXPECT_TRUE(floatsEqual(p1.b, 0.0f));
    EXPECT_TRUE(floatsEqual(p1.a, 1.0f));
}

TEST(Pixel, PixelRGBAArithmetics)
{
    PixelRGBA p = {1.0f, 2.0f, 3.0f, 4.0f};
    p += PixelRGBA{1.0f, 1.0f, 1.0f, 1.0f};
    EXPECT_TRUE((p == PixelRGBA{2.0f, 3.0f, 4.0f, 5.0f}));

    p -= PixelRGBA{1.0f, 1.0f, 1.0f, 1.0f};
    EXPECT_TRUE((p == PixelRGBA{1.0f, 2.0f, 3.0f, 4.0f}));

    p *= PixelRGBA{2.0f, 2.0f, 2.0f, 0.0f};
    EXPECT_TRUE((p == PixelRGBA{2.0f, 4.0f, 6.0f, 0.0f}));

    p /= PixelRGBA{2.0f, 2.0f, 0.0f, 0.0f};
    EXPECT_TRUE((p == PixelRGBA{1.0f, 2.0f, Inf, Nan})); // INF and NAN comparison

    p = PixelRGBA{1.0f, 2.0f, 3.0f, 4.0f};

    p += 1.5f;
    EXPECT_TRUE((p == PixelRGBA{2.5f, 3.5f, 4.5f, 5.5f}));

    p -= 1.5;
    EXPECT_TRUE((p == PixelRGBA{1.0f, 2.0f, 3.0f, 4.0f}));

    p *= 2;
    EXPECT_TRUE((p == PixelRGBA{2.0f, 4.0f, 6.0f, 8.0f}));

    p /= 2;
    EXPECT_TRUE((p == PixelRGBA{1.0f, 2.0f, 3.0f, 4.0f}));
}
