#include <gtest/gtest.h>

#include <dmxdenoiser/FilterKernels.hpp>

#include <iostream>

using namespace dmxdenoiser;

TEST(FilterKernels, ToString)
{
    Kernel2D kernel = getBoxKernel(3);
    std::cout << kernel.ToString() << '\n';
}
