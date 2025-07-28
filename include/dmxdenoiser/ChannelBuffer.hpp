#ifndef DMXDENOISER_CHANNEL_BUFFER_H
#define DMXDENOISER_CHANNEL_BUFFER_H

#include <dmxdenoiser/PixelType.hpp>

#include <cstddef>
#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

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

        PixelDataContainer(PixelType type, std::size_t size)
        {
            allocate(type, size);
        }
    };

    class ChannelBuffer
    {
    public:
        ChannelBuffer(const std::string& name, PixelType pixelType, std::size_t size)
            : m_name{name}, m_pixelType{pixelType}, m_data{PixelDataContainer{pixelType, size}} {}
        ChannelBuffer(const ChannelBuffer&) = delete;
        ChannelBuffer operator=(const ChannelBuffer&) = delete;
        ~ChannelBuffer() = default;

        const std::string& getName() const { return m_name; };

        char* getRawPtr()
        {
            switch(m_pixelType)
            {
            case PixelType::UInt8:    return reinterpret_cast<char*>(m_data.uint8s.data());
            case PixelType::UInt16:   return reinterpret_cast<char*>(m_data.uint16s.data());
            case PixelType::UInt32:   return reinterpret_cast<char*>(m_data.uint32s.data());
            case PixelType::Half:     return reinterpret_cast<char*>(m_data.halfs.data());
            case PixelType::Float:    return reinterpret_cast<char*>(m_data.floats.data());
            case PixelType::Double:   return reinterpret_cast<char*>(m_data.doubles.data());
            default:                  throw std::runtime_error("Unsupported pixel type");
            }
        }

        PixelDataContainer& data() { return m_data; }

    private:
        std::string m_name{};
        PixelType m_pixelType = PixelType::Unknown;
        PixelDataContainer m_data;
    };

    /// @brief Stores per-layer channel buffers: layer name -> list of ChannelBuffer (e.g. R, G, B, A)
    using LayerChannelBufferMap = std::map<std::string, std::vector<ChannelBuffer>>;
}

#endif
