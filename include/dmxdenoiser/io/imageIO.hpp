#ifndef DMXDENOISER_IO_IIMAGEIO_H
#define DMXDENOISER_IO_IIMAGEIO_H

#include <dmxdenoiser/image/Image.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <memory>

namespace dmxdenoiser::io
{
    struct ImageIOParams
    {
        virtual ~ImageIOParams() = default;
    };

    class ImageIO
    {
    public:
        ImageIO(std::string_view filename, std::unique_ptr<ImageIOParams> params)
            : m_filename{ filename }, m_params{ std::move(params) }
        {}

        virtual bool read(Image& img) = 0;
        virtual bool write(const Image& img) const = 0;

        virtual ~ImageIO() = default;

    protected:
        std::string m_filename{};
        std::unique_ptr<ImageIOParams> m_params;
    };
}

#endif
