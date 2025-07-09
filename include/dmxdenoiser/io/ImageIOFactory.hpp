#ifndef DMXDENOISER_IO_IMAGEIO_FACTORY_H
#define DMXDENOISER_IO_IMAGEIO_FACTORY_H

#include <dmxdenoiser/io/ImageIO.hpp>
#include <string>
#include <vector>
#include <memory>

namespace dmxdenoiser::io
{
    std::string getFileExtension(std::string_view filename);
    std::unique_ptr<ImageIO> createImageIO(std::string_view filename);
}

#endif
