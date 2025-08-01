// TestImage.hpp
#pragma once

#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Pixel.hpp>

#include <vector>

namespace dmxdenoiser
{

    class TestImage : public DMXImage
    {
        int w = 3; 
        int h = 3;

        std::vector<PixelRGBA> 
    };

} // namespace dmxdenoiser
