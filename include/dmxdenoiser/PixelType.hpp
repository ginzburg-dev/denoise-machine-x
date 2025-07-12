#ifndef DMXDENOISER_IMAGE_PIXEL_TYPE_H
#define DMXDENOISER_IMAGE_PIXEL_TYPE_H

#include <OpenEXR/ImfPixelType.h>

namespace dmxdenoiser
{
    enum class PixelType {
        Unknown = 0,
        UInt8,      // 8-bit integer (PNG, JPG, TIFF)
        UInt10,     // 10-bit integer (DPX, RAW)
        UInt12,     // 12-bit integer (RAW, DPX)
        UInt14,     // 14-bit integer (RAW)
        UInt16,     // 16-bit integer (PNG, TIFF)
        UInt32,     // 32-bit integer (EXR masks)
        Half,       // 16-bit float (OpenEXR)
        Float,      // 32-bit float (EXR, TIFF)
        Double,      // 64-bit float (rare, TIFF)
        MAX_TYPE,
    };

    inline Imf::PixelType toEXRPixelType(PixelType t) 
    {
        switch (t)
        {
        case PixelType::Float: return Imf::FLOAT;
        case PixelType::Half: return Imf::HALF;
        case PixelType::UInt32: return Imf::UINT;
        default: throw std::runtime_error("Unsupported channel pixel type for OpenEXR");
        }
    }

    /// @brief Convert OpenEXR Imf::PixelType to dmxdenoiser::PixelType. Overloaded conversion function
    inline PixelType toPixelType(Imf::PixelType t) 
    {
        switch (t)
        {
        case Imf::FLOAT: return PixelType::Float;
        case Imf::HALF: return PixelType::Half;
        case Imf::UINT: return PixelType::UInt32;
        default: throw std::runtime_error("Unsupported channel pixel type for dmxdenoiser");
        }
    }

    inline int toPngBitDepth(PixelType t) {
        switch (t) 
        {
        case PixelType::UInt8:  return 8;
        case PixelType::UInt16: return 16;
        default: throw std::runtime_error("Unsupported pixel type for PNG");
        }
    }
}

#endif // DMXDENOISER_IMAGE_DMX_INFO_TYPES_H
