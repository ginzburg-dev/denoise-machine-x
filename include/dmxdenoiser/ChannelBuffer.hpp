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
#include <OpenEXR/ImfPixelType.h>

namespace dmxdenoiser
{

    struct PixelDataContainer
    {
        std::vector<uint8_t> uint8s{};
        std::vector<uint16_t> uint16s{};
        std::vector<uint32_t> uint32s{};
        std::vector<half> halfs{};
        std::vector<float> floats{};
        std::vector<double> doubles{};

        void allocate(PixelType type, std::size_t size)
        {
            switch(type)
            {
                case PixelType::UInt8:    uint8s.resize(size);
                case PixelType::UInt16:   uint16s.resize(size);
                case PixelType::UInt32:   uint32s.resize(size);
                case PixelType::Half:     halfs.resize(size);
                case PixelType::Float:    floats.resize(size);
                case PixelType::Double:   doubles.resize(size);
                default:                  break;
            }
        }
        /*
        char* get(PixelType type){
            swith(type)
            {
                case PixelType::UInt8:    return reinterpret_cast<char*>(uint8s.data());
                case PixelType::UInt16:   return reinterpret_cast<char*>(uint16s.data());
                case PixelType::UInt32:   return reinterpret_cast<char*>(uint32s.data());
                case PixelType::Half:     return reinterpret_cast<char*>(halfs.data());
                case PixelType::Float:    return reinterpret_cast<char*>(floats.data());
                case PixelType::Double:   return reinterpret_cast<char*>(doubles.data());
            }
        }
        */

        PixelDataContainer(PixelType type, std::size_t size)
        {
            allocate(type, size);
        }
    };
    /*
    inline PixelDataContainer allocatePixelData(PixelType type, std::size_t count) {
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
    */
    struct ChannelBuffer
    {
        std::string name{};
        PixelType pixelType = PixelType::Unknown;
        PixelDataContainer data;

        char* get(PixelType type) const
        {
            case PixelType::UInt8:    uint8s.resize(size);
                case PixelType::UInt16:   uint16s.resize(size);
                case PixelType::UInt32:   uint32s.resize(size);
                case PixelType::Half:     halfs.resize(size);
                case PixelType::Float:    floats.resize(size);
                case PixelType::Double:   doubles.resize(size);
                default:                  break;
        }

        ChannelBuffer(const std::string& name_, PixelType pixelType_, std::size_t size)
            : name{name_}, pixelType{pixelType_}, data{PixelDataContainer{pixelType_, size}} {}
        /*
        static ChannelBuffer create(const std::string& name, PixelType pixelType, std::size_t size)
        {
            ChannelBuffer buf;
            buf.name = name;
            buf.pixelType = pixelType;
            buf.data.resize(size);
            return buf;
        }
        */
    };

    /// @brief Stores per-layer channel buffers: layer name -> list of ChannelBuffer (e.g. R, G, B, A)
    using LayerChannelBufferMap = std::unordered_map<std::string, std::vector<ChannelBuffer>>;

}

#endif
