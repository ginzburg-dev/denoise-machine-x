#ifndef DMXDENOISER_CHANNEL_BUFFER_H
#define DMXDENOISER_CHANNEL_BUFFER_H

#include <dmxdenoiser/PixelType.hpp>
#include <variant>
#include <string>
#include <string_view>
#include <cstddef>

namespace dmxdenoiser
{

    struct ChannelBuffer
    {
        std::string name{};
        PixelType pixelType = PixelType::Unknown;
        PixelDataVariant data{};
        
        template<typename T>
        static ChannelBuffer create(const std::string& name, std::vector<T> pixels)
        {
            ChannelBuffer buf;
            buf.name = name;
            buf.pixelType = PixelTypeOf<T>;
            buf.data = std::move(pixels);
            return buf;
        }

        template<typename T>
        static ChannelBuffer createEmpty(const std::string& name, T pixelType, std::size_t size)
        {
            ChannelBuffer buf;
            buf.name = name;
            buf.pixelType = toPixelType(T);
            buf.data = allocatePixelData(toPixelType(T), size);
            return buf;
        }
    };

    /// @brief Buffer Dictionary: [layer][ [R buffer], [G buffer], [B buffer], ... ]
    using ChannelBufferDictionary = std::unordered_map<std::string, std::vector<ChannelBuffer>>;

}

#endif
