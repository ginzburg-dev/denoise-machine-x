#include <dmxdenoiser/ImageIO.hpp>
#include <dmxdenoiser/ImageIOFactory.hpp>

#include <algorithm>
#include <string_view>

namespace dmxdenoiser
{

    std::unique_ptr<ImageIO> createImageIO(const std::string& filename)
    {
        ImageFileType imageType = getImageFileType(filename);

        if ( imageType == ImageFileType::EXR )
        {
            return std::make_unique<ExrImageIO>();
        }

        if (  imageType == ImageFileType::JPG || imageType == ImageFileType::JPEG )
        {
            //return std::make_unique<JpgImageIO>();
        }

        if ( imageType == ImageFileType::PNG )
        {
            //return std::make_unique<PngImageIO>();
        }

        return nullptr;
    }
    
} // namespace dmxdenoiser

