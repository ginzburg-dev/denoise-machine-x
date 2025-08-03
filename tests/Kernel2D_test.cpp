#include <gtest/gtest.h>

#include <dmxdenoiser/Kernel2D.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace dmxdenoiser;

TEST(Kernel2D, CopyConstructor)
{
    std::vector<float> data(9, 1.0f);
    Kernel2D kernel1("Kernel", data);
    Kernel2D kernel2{kernel1};

    EXPECT_EQ(kernel1.name(), kernel2.name());
    EXPECT_EQ(kernel1.size(), kernel2.size());

    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
        {
            EXPECT_FLOAT_EQ(kernel1(y, x), kernel2(y, x));
        }
}

TEST(Kernel2D, Construct3x3)
{
    std::vector<float> data(9, 1.0f);
    Kernel2D kernel1("Kernel1", data);
    Kernel2D kernel2{};
    kernel2.set("Kernel2", data);
    EXPECT_EQ(kernel1.size(), 3);
    EXPECT_EQ(kernel2.size(), 3);
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
        {
            EXPECT_FLOAT_EQ(kernel1(y, x), 1.0f);
            EXPECT_FLOAT_EQ(kernel2(y, x), 1.0f);
        }
}

TEST(Kernel2D, ThrowOnNonSquare)
{
    std::string name = "Kernel";
    std::vector<float> data(6, 1.0f);
    EXPECT_THROW(Kernel2D kernel(name, data), std::invalid_argument);
    EXPECT_THROW({Kernel2D kernel{}; kernel.set(name, data);}, std::invalid_argument);
}

TEST(Kernel2D, ThrowOnEmpty)
{
    std::string name = "Kernel";
    std::vector<float> data{};
    EXPECT_THROW(Kernel2D kernel(name, data), std::invalid_argument);
    EXPECT_THROW({Kernel2D kernel{}; kernel.set(name, data);}, std::invalid_argument);
}

TEST(Kernel2D, ThrowOnEven)
{
    std::string name = "Kernel";
    std::vector<float> data(2, 1.0f);
    EXPECT_THROW(Kernel2D kernel(name, data), std::invalid_argument);
}

TEST(Kernel2D, ThrowOnOutOfBound)
{
    std::string name = "Kernel";
    std::vector<float> data(9, 1.0f);
    Kernel2D kernel(name, data);
    EXPECT_THROW(kernel(3, 0), std::invalid_argument);
    EXPECT_THROW(kernel(0, 3), std::invalid_argument);
}

TEST(Kernel2D, ToString)
{
    std::string name = "UniqueKernelName";
    std::vector<float> data(9, 2.0f);
    Kernel2D kernel(name, data);
    std::string str = kernel.ToString();
    std::cout << str << '\n';
    EXPECT_TRUE(str.find("UniqueKernelName") != std::string::npos);
    EXPECT_TRUE(str.find("3 x 3") != std::string::npos);
    EXPECT_TRUE(str.find("2.0000") != std::string::npos);
}
