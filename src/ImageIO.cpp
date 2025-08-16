#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/ImageFileType.hpp>
#include <dmxdenoiser/ImageIO.hpp>
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
    
    std::string ImageInfo::ToString() const
    {
        std::ostringstream oss;
        oss << "ImageInfo: \n";
        oss << "    Dimensions: " << width << " x " << height << '\n';
        oss << "    ImageFileType: " << type << '\n';
        oss << "    Layers: ";
        for (const auto& i : layers.data())
        {
            oss << i.first << ' ';
        }
        return oss.str();
    }

} // namespace dmxdenoiser
