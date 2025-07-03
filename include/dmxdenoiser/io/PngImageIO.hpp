#ifndef DMXDENOISER_IO_PNGIMAGEIO_H
#define DMXDENOISER_IO_PNGIMAGEIO_H

#include <dmxdenoiser/io/ImageIO.hpp>
#include <dmxdenoiser/image/Image.hpp>
#include <string>
#include <string_view>

namespace dmxdenoiser::io
{
    struct PngIOParams : public ImageIOParams
    {
        int bitDepth = 16;
        bool interlaced = false;
        ~PngIOParams() override = default;
    };

    class PngImageIO : public ImageIO
    {
    public:
        PngImageIO(std::string_view filename, std::unique_ptr<ImageIOParams> params)
            : ImageIO(filename, std::move(params))
        {}

        bool read(Image& img) override;
        bool write(const Image& img) const override;

        ~PngImageIO() override = default;
    };
}

#endif
