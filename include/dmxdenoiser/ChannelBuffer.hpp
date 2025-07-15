#ifndef DMXDENOISER_CHANNEL_BUFFER_H
#define DMXDENOISER_CHANNEL_BUFFER_H

#include <dmxdenoiser/PixelType.hpp>

#include <cstddef>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <unordered_map>

#include <Imath/half.h>

namespace dmxdenoiser
{

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

    struct ChannelBuffer
    {
        std::string name{};
        PixelType pixelType = PixelType::Unknown;
        PixelDataVariant data{};
        
        template<typename T>
        static ChannelBuffer create(const std::string& name, std::vector<T>&& pixels)
        {
            ChannelBuffer buf;
            buf.name = name;
            buf.pixelType = PixelTypeOf<T>;
            buf.data = std::move(pixels);
            return buf;
        }

        static ChannelBuffer createEmpty(const std::string& name, PixelType pixelType, std::size_t size)
        {
            ChannelBuffer buf;
            buf.name = name;
            buf.pixelType = pixelType;
            buf.data = allocatePixelData(pixelType, size);
            return buf;
        }
    };

    /// @brief Stores per-layer channel buffers: layer name -> list of ChannelBuffer (e.g. R, G, B, A)
    using LayerChannelBufferMap = std::unordered_map<std::string, std::vector<ChannelBuffer>>;

}

#endif
