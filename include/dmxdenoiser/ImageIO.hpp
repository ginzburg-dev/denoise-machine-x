// ImageIO.hpp
#pragma once

#include <dmxdenoiser/ChannelBuffer.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/PixelType.hpp>
#include <dmxdenoiser/LayerDictionary.hpp>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <Imath/half.h>
#include <OpenEXR/ImfPixelType.h>
#include <OpenEXR/ImfCompression.h>

namespace dmxdenoiser
{

    std::string getFileExtension(std::string_view filename);

    enum class ImageFileType
    {
        Unknown = 0,
        EXR,
        JPG,
        JPEG,
        PNG,
    };

    ImageFileType getImageFileType(const std::string& filename);

    struct ImageInfo
    {
        int width{};
        int height{};
        ImageFileType type{};
        LayerDictionary layers{};
        ParamDictionary params{};
    };

    /*Abstract*/
    class ImageIO
    {
    public:
        ImageIO() = default;

        virtual bool read(
            const std::string& filename,
            DMXImage& img,
            const AovDictionary& layers) = 0;

        virtual bool write(
            const std::string& filename,
            DMXImage& img,
            const AovDictionary& layers) const = 0;

        virtual ImageInfo getImageInfo(const std::string& filename) const = 0;
        
        virtual ~ImageIO() = default;
    };

    class ExrImageIO : public ImageIO
    {
    public:
        ExrImageIO() = default;

        bool read(
            const std::string& filename,
            DMXImage& img,
            const AovDictionary& layers) override;

        bool write(
            const std::string& filename,
            DMXImage& img,
            const AovDictionary& layers) const override;

        ImageInfo getImageInfo(const std::string& filename) const override;

        ~ExrImageIO() override;
    };

} // namespace dmxdenoiser
