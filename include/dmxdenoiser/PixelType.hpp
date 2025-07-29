#ifndef DMXDENOISER_IMAGE_PIXEL_TYPE_H
#define DMXDENOISER_IMAGE_PIXEL_TYPE_H

#include <cstdint>
#include <cstddef>

#include <Imath/half.h>
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

    /// @brief Compile-time C++ type to PixelType mapping (type trait).
    template<typename T> inline constexpr PixelType PixelTypeOf = PixelType::Unknown;
    template<> inline constexpr PixelType PixelTypeOf<uint8_t> = PixelType::UInt8;
    template<> inline constexpr PixelType PixelTypeOf<uint16_t> = PixelType::UInt16;
    template<> inline constexpr PixelType PixelTypeOf<uint32_t> = PixelType::UInt32;
    template<> inline constexpr PixelType PixelTypeOf<half> = PixelType::Half;
    template<> inline constexpr PixelType PixelTypeOf<float> = PixelType::Float;
    template<> inline constexpr PixelType PixelTypeOf<double> = PixelType::Double;

    /// @brief Convert dmxdenoiser::PixelType to OpenEXR Imf::PixelType (runtime).
    /*inline*/ constexpr Imf::PixelType toEXRPixelType(PixelType t) 
    {
        switch (t)
        {
        case PixelType::Float: return Imf::FLOAT;
        case PixelType::Half: return Imf::HALF;
        case PixelType::UInt32: return Imf::UINT;
        default: throw std::runtime_error("Unsupported channel pixel type for OpenEXR");
        }
    }

    /// @brief Identity overload for PixelType (runtime).
    /*inline*/ constexpr PixelType toPixelType(PixelType t) 
    {
        return t;
    }

    /// @brief Convert OpenEXR Imf::PixelType to dmxdenoiser::PixelType. 
    /*inline*/ constexpr PixelType toPixelType(Imf::PixelType t) 
    {
        switch (t)
        {
        case Imf::FLOAT: return PixelType::Float;
        case Imf::HALF: return PixelType::Half;
        case Imf::UINT: return PixelType::UInt32;
        default: throw std::runtime_error("Unsupported channel pixel type for dmxdenoiser");
        }
    }

    /// @brief Get PNG bit depth from PixelType (runtime).
    inline int toPngBitDepth(PixelType t) {
        switch (t) 
        {
        case PixelType::UInt8:  return 8;
        case PixelType::UInt16: return 16;
        default: throw std::runtime_error("Unsupported pixel type for PNG");
        }
    }

    inline std::string ToString(PixelType type)
    {   
        switch (type)
        {
        case PixelType::UInt8:  return "UInt8";
        case PixelType::UInt10: return "UInt10";
        case PixelType::UInt12: return "UInt12";
        case PixelType::UInt14: return "UInt14";
        case PixelType::UInt16: return "UInt16";
        case PixelType::UInt32: return "UInt32";
        case PixelType::Half:   return "Half";
        case PixelType::Float:  return "Float";
        case PixelType::Double: return "Double";
        case PixelType::Unknown:
        default:                return "Unknown";
        }
    }

} // namespace dmxdenoiser

#endif // DMXDENOISER_IMAGE_PIXEL_TYPE_H
