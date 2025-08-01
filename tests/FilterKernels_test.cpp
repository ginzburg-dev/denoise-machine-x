#include <gtest/gtest.h>

#include <dmxdenoiser/FilterKernels.hpp>

#include <iostream>
#include <vector>

using namespace dmxdenoiser;

TEST(FilterKernels, BoxKernel)
{
    Kernel2D kernel = FilterKernels::getBoxKernel(3);
    std::vector<float> kernelRef(9, 1.0f / 9.0f);
    EXPECT_EQ(kernel.m_data, kernelRef);
}

TEST(FilterKernels, ToString)
{
    Kernel2D kernel = FilterKernels::getBoxKernel(3);
    std::cout << kernel.ToString() << '\n';
}
