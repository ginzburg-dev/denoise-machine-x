#ifndef DMXDENOISER_IO_IMAGEIO_H
#define DMXDENOISER_IO_IMAGEIO_H

#include <string>
#include <string_view>

namespace dmxdenoiser::io
{
    class IImageIO
    {
    public:
        IImageIO() = default;
        virtual bool load(std::string_view filename) = 0;
        virtual bool save(std::string_view filename) const = 0;
        virtual ~IImageIO() = default;
    };
}
#endif
