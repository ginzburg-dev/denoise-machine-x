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

#include <OpenEXR/ImfPixelType.h>

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

    inline constexpr std::string_view ToString(ImageFileType type)
    {
        switch (type)
        {
        case ImageFileType::EXR:  return "Exr";
        case ImageFileType::JPEG : return "Jpeg";
        case ImageFileType::JPG : return "Jpg";
        case ImageFileType::PNG : return "Png";
        default:                return "Unknown";
        }
    }

    inline std::ostream& operator<<(std::ostream& out, ImageFileType type)
    {
        out << ToString(type);
        return out;
    }

    struct ExrOutputChannel
    {
        std::string name{};
        Imf::PixelType pixelType{};
        char* ptr = nullptr;
        ExrOutputChannel(const std::string& name_, Imf::PixelType pixelType_, char* ptr_)
            : name{name_}, pixelType{pixelType_}, ptr{ptr_}
        {}
    };

    ImageFileType getImageFileType(const std::string& filename);

    void copyChannelBuffersToDMXImage(
        const std::vector<ChannelBuffer>& buff,
        std::string_view layer, 
        int frame, 
        DMXImage& img);

    std::vector<ChannelBuffer> copyDMXImageToChannelBuffers(
        const DMXImage& img,
        const std::vector<std::string>& layers
        );

    struct ImageInfo
    {
        int width{};
        int height{};
        ImageFileType type{};
        LayerDictionary layers{};
        ParamDictionary params{};

        std::string ToString() const;
    };

    inline std::ostream& operator<<(std::ostream& out, ImageInfo info)
    {
        out << info.ToString();
        return out;
    }

    /*Abstract*/
    class ImageIO
    {
    public:
        ImageIO() = default;

        virtual void read(
            const std::string& filename,
            DMXImage& img,
            int frame,
            const AovDictionary& layers,
            int numChannels = DEFAULT_NUM_CHANNELS) = 0;

        virtual void write(
            const std::string& filename,
            const DMXImage& img,
            const std::vector<std::string>& layers) const = 0;

        virtual ImageInfo getImageInfo(const std::string& filename) const = 0;
        
        virtual ~ImageIO() = default;
    };

    class ExrImageIO : public ImageIO
    {
    public:
        ExrImageIO() = default;

        void read(
            const std::string& filename,
            DMXImage& img,
            int frame,
            const AovDictionary& layers,
            int numChannels = DEFAULT_NUM_CHANNELS) override;

        void write(
            const std::string& filename,
            const DMXImage& img,
            const std::vector<std::string>& layers) const override;

        ImageInfo getImageInfo(const std::string& filename) const override;

        ~ExrImageIO() override;
    };

} // namespace dmxdenoiser
