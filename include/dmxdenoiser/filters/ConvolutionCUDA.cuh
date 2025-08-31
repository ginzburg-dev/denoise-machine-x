#pragma once

#include <dmxdenoiser/Config.hpp>
#include <dmxdenoiser/DMXImageView.hpp>

namespace dmxdenoiser
{
    void convolve2D_CUDA(DMXImageView in, DMXImageView out, int* frames, int framesSize, int* layers, int layersSize,
                                    float* kernel, int kernelSize, float strength, bool filterAlpha);
} // namespace dmxdenoiser
