#ifndef DMXDENOISER_IO_IIMAGEIO_H
#define DMXDENOISER_IO_IIMAGEIO_H

#include <dmxdenoiser/image/DMXImage.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <functional>

namespace dmxdenoiser::io
{

enum class ExrCompression { None, PIZ, ZIPS, ZIP, RLE, PXR24, B44, B44A, DWAA, DWAB };

enum class PixelType { UINT = 0, HALF = 1, FlOAT = 3 };

struct ImageInfo
{
    int width{};
    int height{};
    std::vector<std::string> layers{}; // optional for EXR format
};

/*Abstract*/
struct ImageIOParams
{
    virtual ~ImageIOParams() = default;
};

struct ExrLayer
{
    std::string name{};
    PixelType pixelType = PixelType::HALF;
};

struct ExrIOParams : public ImageIOParams
{
    ExrCompression compression = ExrCompression::None;
    std::vector<std::string> layers{};
    ~ExrIOParams() override = default;
};

/*Abstract*/
class ImageIO
{
public:
    ImageIO() = default;

    virtual bool read(std::string_view filename, float* img, const ImageIOParams* params) = 0;

    virtual bool write(
        std::string_view filename,
        const float* img, /*first element array pointer to write*/
        int width,
        int height,
        int channels, /*RGBA RGB GREY*/
        const ImageIOParams* params = nullptr) const = 0;

    virtual ImageInfo getImageInfo(std::string_view filename) = 0;

    virtual ~ImageIO() = default;
};

class ExrIO : public ImageIO
{
public:
    ExrIO() = default;

    /*virtual*/bool read(std::string_view filename, float* img, const ImageIOParams* params) override;

    /*virtual*/bool write(
        std::string_view filename,
        const float* img,
        int width,
        int height,
        int channels,
        const ImageIOParams* params = nullptr) const override;

    ImageInfo getImageInfo(std::string_view filename) override;
    ~ExrIO() override;
};

}

#endif
