#ifndef DMXDENOISER_IMAGE_IO_H
#define DMXDENOISER_IMAGE_IO_H

#include <dmxdenoiser/ChannelBuffer.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/LayerDictionary.hpp>
#include <dmxdenoiser/PixelType.hpp>

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
    
    std::string toLower(const std::string& s);

    /*Abstract*/
    struct ImageInfo
    {
        int width{};
        int height{};
        virtual ~ImageInfo() = default;
    };

    struct ExrImageInfo : public ImageInfo
    {
        std::vector<LayerInfo> layers{};
        Imf::Compression compression{};
        int channelCount() const;
        ~ExrImageInfo () override = default;
    };

    /*Abstract*/
    struct ImageIOParams
    {
        virtual ~ImageIOParams() = default;
    };

    struct ExrIOParams : public ImageIOParams
    {
        Imf::Compression compression = Imf::NO_COMPRESSION; // for writing EXR image
        std::vector<LayerInfo> layers{}; // { {layer1, { {channel1, type}, {channel2, type} } }, ...}
        int channelCount() const;
        ~ExrIOParams() override = default;
    };

    /*Abstract*/
    class ImageIO
    {
    public:
        ImageIO() = default;

        virtual bool read(
            std::string_view filename,
            float* img,
            const ImageIOParams* params) = 0;

        virtual bool write(
            std::string_view filename,
            const float* img,
            const ImageIOParams* params) const = 0;

        virtual std::unique_ptr<ImageInfo> getImageInfo(std::string_view filename) const = 0;
        
        virtual ~ImageIO() = default;
    };

    class ExrImageIO : public ImageIO
    {
    public:
        ExrImageIO() = default;

        bool read(
            std::string_view filename,
            float* img,
            const ImageIOParams* params) override;

        bool write(
            std::string_view filename,
            const float* img,
            const ImageIOParams* params) const override;

        std::unique_ptr<ImageInfo> getImageInfo(std::string_view filename) const override;

        ~ExrImageIO() override;
    };

} // namespace dmxdenoiser

#endif // DMXDENOISER_IMAGE_IO_H
