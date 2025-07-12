#ifndef DMXDENOISER_IMAGE_PIXEL_TYPE_H
#define DMXDENOISER_IMAGE_PIXEL_TYPE_H

#include <OpenEXR/ImfPixelType.h>
#include <Imath/half.h>
#include <cstddef>

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

    using PixelDataVariant = std::variant<
        std::monostate,
        std::vector<uint8_t>,
        std::vector<uint16_t>,
        std::vector<uint32_t>,
        std::vector<half>,
        std::vector<float>,
        std::vector<double>
    >;

    inline PixelDataVariant allocatePixelData(PixelType type, std::size_t count) {
        switch (type) {
            case PixelType::UInt8:    return std::vector<uint8_t>(count);
            case PixelType::UInt16:   return std::vector<uint16_t>(count);
            case PixelType::UInt32:   return std::vector<uint32_t>(count);
            case PixelType::Half:     return std::vector<half>(count);
            case PixelType::Float:    return std::vector<float>(count);
            case PixelType::Double:   return std::vector<double>(count);
            default:                  return std::monostate{};
        }
    }

    template<typename T>
    inline constexpr PixelType PixelTypeOf<T> = PixelType::Unknown;
    template<>
    inline constexpr PixelType PixelTypeOf<uint8_t> = PixelType::UInt8;
    template<>
    inline constexpr PixelType PixelTypeOf<uint16_t> = PixelType::UInt16;
    template<>
    inline constexpr PixelType PixelTypeOf<uint32_t> = PixelType::UInt32;
    template<>
    inline constexpr PixelType PixelTypeOf<half> = PixelType::Half;
    template<>
    inline constexpr PixelType PixelTypeOf<float> = PixelType::Float;
    template<>
    inline constexpr PixelType PixelTypeOf<double> = PixelType::Double;


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

    inline PixelType toPixelType(PixelType t) 
    {
        return t;
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
