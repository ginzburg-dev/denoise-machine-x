#include <dmxdenoiser/ImageIO.hpp>
#include <dmxdenoiser/ImageIOFactory.hpp>

#include <algorithm>
#include <string_view>

namespace dmxdenoiser
{
    constexpr std::string_view kFormatEXR = "exr";
    constexpr std::string_view kFormatJPG = "jpg";
    constexpr std::string_view kFormatJPEG = "jpeg";
    constexpr std::string_view kFormatPNG = "exr";

    /*
    std::unique_ptr<ImageIO> createImageIO(std::string_view filename)
    {
        auto ext = getFileExtension(filename);

        if ( ext == kFormatEXR )
        {
            std::unique_ptr<ImageIOParams> params = std::make_unique<ExrIOParams>();
            return std::make_unique<ExrImageIO>();
        }

        if ( ext == kFormatJPG || ext == kFormatJPEG )
        {
            //std::unique_ptr<ImageIOParams> params = std::make_unique<JpgIOParams>();
            //return std::make_unique<JpgImageIO>(filename, std::move(params));
        }

        if ( ext == kFormatPNG )
        {
            //std::unique_ptr<ImageIOParams> params = std::make_unique<PngIOParams>();
            //return std::make_unique<PngImageIO>(filename, std::move(params));
        }

        return nullptr;
    }
    */
} // namespace dmxdenoiser

