#ifndef DMXDENOISER_IO_IIMAGEIO_H
#define DMXDENOISER_IO_IIMAGEIO_H

#include <dmxdenoiser/image/DMXImage.hpp>
#include <OpenEXR/ImfPixelType.h>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace dmxdenoiser::io
{

enum class ExrCompression { None, PIZ, ZIPS, ZIP, RLE, PXR24, B44, B44A, DWAA, DWAB };

enum class PixelType { UINT = 0, HALF = 1, FLOAT = 3 };

std::string toLower(const std::string& s);

struct ExrChannel
{
    std::string name{};
    Imf::PixelType type{};
    ExrChannel(std::string_view name_, Imf::PixelType type_)
        : name{name_}, type{type_}
    {}
};

/*Abstract*/
struct ImageInfo
{
    int width{};
    int height{};
    virtual ~ImageInfo() = default;
};

struct ExrImageInfo : public ImageInfo
{
    //std::map<std::string, std::vector<ExrChannel>> channels{};
    std::vector<DMXLayer> layers{};
    ExrCompression compression{};
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
    ExrCompression compression = ExrCompression::None; // for writing EXR image
    //std::map<std::string, std::vector<ExrChannel>> channels{};
    std::vector<DMXLayer> layers{}; // { {layer1, { {channel1, type}, {channel2, type} } }, ...}
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

}

#endif
