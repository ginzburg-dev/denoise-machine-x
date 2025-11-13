// ImageIOExr.hpp
#pragma once

#include <dmxdenoiser/ImageIO.hpp>
#include <dmxdenoiser/DMXImage.hpp>

#include <string>
#include <string_view>
#include <vector>

#include <OpenEXR/ImfPixelType.h>
#include <OpenEXR/ImfCompression.h>

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

    constexpr std::string_view exrCompressionName(Imf::Compression c) noexcept {
        switch (c) {
            case Imf::NO_COMPRESSION:   return "NONE";
            case Imf::RLE_COMPRESSION:  return "RLE";
            case Imf::ZIPS_COMPRESSION: return "ZIPS (ZIP per scanline)";
            case Imf::ZIP_COMPRESSION:  return "ZIP (multi-scanline)";
            case Imf::PIZ_COMPRESSION:  return "PIZ (wavelet, lossless)";
            case Imf::PXR24_COMPRESSION:return "PXR24 (float->24-bit, lossy for float)";
            case Imf::B44_COMPRESSION:  return "B44 (HALF only, lossy)";
            case Imf::B44A_COMPRESSION: return "B44A (HALF only, lossy, better alpha)";
            case Imf::DWAA_COMPRESSION: return "DWAA (lossy, scanline-oriented)";
            case Imf::DWAB_COMPRESSION: return "DWAB (lossy, stronger than DWAA)";
            default:                    return "UNKNOWN";
        }
    }

    class ImageIOExr : public ImageIO
    {
    public:
        ImageIOExr() = default;
        
        [[nodiscard]] static std::unique_ptr<ImageIOExr> create();
        
        void read(
            const std::string& filename,
            DMXImage& img,
            int frame,
            const AovDictionary& layers) override;

        void write(
            const std::string& filename,
            const DMXImage& img,
            const std::vector<std::string>& layers,
            int frame = 0) const override;

        ImageInfo getImageInfo(const std::string& filename) const override;
        
        ~ImageIOExr() override = default;
    };

} // namespace dmxdenoiser
