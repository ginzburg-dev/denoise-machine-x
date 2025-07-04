#ifndef DMXDENOISER_IO_JPGIMAGEIO_H
#define DMXDENOISER_IO_JPGIMAGEIO_H

#include <dmxdenoiser/io/ImageIO.hpp>
#include <dmxdenoiser/image/Image.hpp>
#include <string>
#include <string_view>

namespace dmxdenoiser::io
{
    struct JpgIOParams : public ImageIOParams
    {
        float quality = 12;
        ~JpgIOParams() override = default;
    };

    class JpgImageIO : public ImageIO
    {
    public:
        JpgImageIO(std::string_view filename, std::unique_ptr<ImageIOParams> params)
            : ImageIO(filename, std::move(params))
        {}

        bool read(Image& img) override;
        bool write(const Image& img) const override;

        ~JpgImageIO() override = default;
    };
}

#endif
