#ifndef DMXDENOISER_IO_EXRIMAGEIO_H
#define DMXDENOISER_IO_EXRIMAGEIO_H

#include <dmxdenoiser/io/ImageIO.hpp>
#include <dmxdenoiser/image/Image.hpp>
#include <string>
#include <string_view>

namespace dmxdenoiser::io
{
    struct ExrIOParams : public ImageIOParams
    {
        std::string channelName;
        ~ExrIOParams() override = default;
    };

    class ExrImageIO : public ImageIO
    {
    public:
        ExrImageIO(std::string_view filename, std::unique_ptr<ImageIOParams> params)
            : ImageIO(filename, std::move(params))
        {}
        bool read(Image& img) override;
        bool write(const Image& img) const override;
        ~ExrImageIO() override = default;
    };
}

#endif
