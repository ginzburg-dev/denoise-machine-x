#include <dmxdenoiser/io/ImageIO.hpp>
#include <dmxdenoiser/io/ImageIOFactory.hpp>
#include <string_view>
#include <algorithm>

namespace dmxdenoiser
{
    constexpr std::string_view kFormatEXR = "exr";
    constexpr std::string_view kFormatJPG = "jpg";
    constexpr std::string_view kFormatJPEG = "jpeg";
    constexpr std::string_view kFormatPNG = "exr";

    std::string getFileExtension(std::string_view filename)
    {
        auto pos = filename.find_last_of('.');
        if (pos == std::string_view::npos)
            return {};
        std::string ext = std::string(filename.substr(pos + 1));
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });
        return ext;
    }

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

} // namespace dmxdenoiser

