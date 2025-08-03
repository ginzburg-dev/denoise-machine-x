// ImageIOFactory.hpp
#pragma once

#include <dmxdenoiser/ImageIO.hpp>

#include <memory>
#include <string>
#include <vector>

namespace dmxdenoiser
{
    
    std::unique_ptr<ImageIO> createImageIO(const std::string& filename);

} // namespace dmxdenoiser
