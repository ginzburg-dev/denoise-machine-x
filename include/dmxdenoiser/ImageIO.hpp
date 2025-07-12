#ifndef DMXDENOISER_IMAGE_IO_H
#define DMXDENOISER_IMAGE_IO_H

#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/LayerDictionary.hpp>
#include <OpenEXR/ImfPixelType.h>
#include <OpenEXR/ImfCompression.h>
#include <Imath/half.h>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <variant>

namespace dmxdenoiser
{

    std::string toLower(const std::string& s);

    struct ExrChannelBuffer
    {
        std::string name{};
        std::optional<Imf::PixelType> pixelType{};
        std::variant<std::monostate, std::vector<float>, std::vector<half>, std::vector<unsigned int>> data{};
    };

    using ExrChannelBuffersMap = std::unordered_map<std::string, std::vector<ExrChannelBuffer>>;

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
