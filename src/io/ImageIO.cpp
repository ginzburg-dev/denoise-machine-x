#include <dmxdenoiser/io/ImageIO.hpp>

namespace dmxdenoiser::io
{

bool ExrIO::read(std::string_view filename, float* img, const ImageIOParams* params)
{
    return true; // pass
}

bool ExrIO::write(
    std::string_view filename,
    const float* img,
    int width,
    int height,
    int channels,
    const ImageIOParams* params) const
{
    return true; // pass
}

ImageInfo ExrIO::getImageInfo(std::string_view filename)
{
    return ImageInfo{}; // pass
}

ExrIO::~ExrIO(){}

}
