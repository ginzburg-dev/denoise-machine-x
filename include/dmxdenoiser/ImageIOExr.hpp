// ImageIOExr.hpp
#pragma once

#include <dmxdenoiser/ImageIO.hpp>
#include <dmxdenoiser/DMXImage.hpp>

#include <string>
#include <string_view>
#include <vector>

#include <OpenEXR/ImfPixelType.h>

namespace dmxdenoiser
{

    struct ExrOutputChannel
    {
        std::string name{};
        Imf::PixelType pixelType{};
        char* ptr = nullptr;
        ExrOutputChannel(const std::string& name_, Imf::PixelType pixelType_, char* ptr_)
            : name{name_}, pixelType{pixelType_}, ptr{ptr_}
        {}
    };

    class ImageIOExr : public ImageIO
    {
    public:
        ImageIOExr() = default;

        void read(
            const std::string& filename,
            DMXImage& img,
            int frame,
            const AovDictionary& layers) override;

        void write(
            const std::string& filename,
            const DMXImage& img,
            const std::vector<std::string>& layers) const override;

        ImageInfo getImageInfo(const std::string& filename) const override;

        ~ImageIOExr() override;
    };

} // namespace dmxdenoiser
