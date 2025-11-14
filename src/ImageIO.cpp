#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/ImageFileType.hpp>
#include <dmxdenoiser/ImageIO.hpp>
#include <dmxdenoiser/ImageIOExr.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/PixelType.hpp>
#include <dmxdenoiser/StringConversions.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <unordered_map>
#include <utility>

#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfPixelType.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfArray.h>
#include <Imath/half.h>
#include <Imath/ImathBox.h>

namespace dmxdenoiser
{

    std::unique_ptr<ImageIO> ImageIO::create(const std::string& filename)
    {
        if(!std::filesystem::exists(filename)) {
            DMX_LOG_ERROR("ImageIO", "create(): File \"", filename, "\" not found.");
            throw std::runtime_error("File \"" + filename + "\" not found.");
        }
        ImageFileType imageType = getImageFileType(filename);
        if ( imageType == ImageFileType::EXR ) { return ImageIOExr::create(); }
        if ( imageType == ImageFileType::JPG || imageType == ImageFileType::JPEG ) { /*in progress*/ }
        if ( imageType == ImageFileType::PNG ) { /*in progress*/ }
        return nullptr;
    }
    
    std::string ImageInfo::ToString() const
    {
        std::ostringstream oss;
        oss << "ImageInfo: \n";
        oss << "\tDimensions: " << width << " x " << height << '\n';
        oss << "\tImageFileType: " << type << '\n';
        oss << "\tLayers: ";
        for (const auto& i : layers.data())
        {
            oss << i.first << ' ';
        }
        return oss.str();
    }

} // namespace dmxdenoiser
