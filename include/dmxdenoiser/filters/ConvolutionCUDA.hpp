// ConvolutionCUDA.hpp
#pragma once

#include <dmxdenoiser/DMXImageView.hpp>

namespace dmxdenoiser
{

    void convolve2D_CUDA(const DMXImage& in_, DMXImage& out_, const std::vector<int>& frames_, const std::vector<int>& layers_,
                                    const Kernel2D& kernel_, float strength, bool filterAlpha);

} // namespace dmxdenoiser
