#ifndef DMXDENOISER_IO_IMAGEIO_FACTORY_H
#define DMXDENOISER_IO_IMAGEIO_FACTORY_H

#include <dmxdenoiser/ImageIO.hpp>

#include <memory>
#include <string>
#include <vector>

namespace dmxdenoiser
{
    
    std::unique_ptr<ImageIO> createImageIO(const std::string& filename);

}

#endif
