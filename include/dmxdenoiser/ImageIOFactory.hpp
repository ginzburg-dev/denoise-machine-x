#ifndef DMXDENOISER_IO_IMAGEIO_FACTORY_H
#define DMXDENOISER_IO_IMAGEIO_FACTORY_H

#include <dmxdenoiser/ImageIO.hpp>

#include <memory>
#include <string>
#include <vector>

namespace dmxdenoiser::io
{
    
    std::unique_ptr<ImageIO> createImageIO(std::string_view filename);
}

#endif
