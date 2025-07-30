#ifndef DMXDENOISER_CHANNEL_BUFFER_H
#define DMXDENOISER_CHANNEL_BUFFER_H

#include <dmxdenoiser/PixelType.hpp>

#include <algorithm>
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
                case PixelType::UInt8:    uint8s.resize(size); break;
                case PixelType::UInt16:   uint16s.resize(size); break;
                case PixelType::UInt32:   uint32s.resize(size); break;
                case PixelType::Half:     halfs.resize(size); break;
                case PixelType::Float:    floats.resize(size); break;
                case PixelType::Double:   doubles.resize(size); break;
                default:                  throw std::runtime_error("Unsupported pixel type. Cannot allocate channel buffer.");
            }
        }

        void clear() { 
            std::fill(uint8s.begin(), uint8s.end(), 0);
            std::fill(uint16s.begin(), uint16s.end(), 0);
            std::fill(uint32s.begin(), uint32s.end(), 0);
            std::fill(halfs.begin(), halfs.end(), half(0.0f));
            std::fill(floats.begin(), floats.end(), 0.0f);
            std::fill(doubles.begin(), doubles.end(), 0.0);
        }

        PixelDataContainer() = default;
        PixelDataContainer(PixelType type, std::size_t size)
        {
            allocate(type, size);
        }
    };

    class ChannelBuffer
    {
    public:
        ChannelBuffer() = default;
        ChannelBuffer(const std::string& name, PixelType pixelType, std::size_t size)
            : m_name{name}, m_pixelType{pixelType}, m_data{PixelDataContainer{pixelType, size}} {}

        ChannelBuffer(const ChannelBuffer&) noexcept = delete;
        ChannelBuffer operator=(const ChannelBuffer&) noexcept = delete;

        ChannelBuffer(ChannelBuffer&&) = default;
        ChannelBuffer& operator=(ChannelBuffer&&) = default;

        ~ChannelBuffer() = default;

        void initialize(const std::string& name, PixelType pixelType, std::size_t size)
        {
            m_name = name;
            m_pixelType = pixelType;
            m_data.allocate(pixelType, size);
        }

        const std::string& getName() const { return m_name; };
        PixelType getPixelType() const { return m_pixelType; };

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

        const char* getRawPtr() const
        {
            switch(m_pixelType)
            {
            case PixelType::UInt8:    return reinterpret_cast<const char*>(m_data.uint8s.data());
            case PixelType::UInt16:   return reinterpret_cast<const char*>(m_data.uint16s.data());
            case PixelType::UInt32:   return reinterpret_cast<const char*>(m_data.uint32s.data());
            case PixelType::Half:     return reinterpret_cast<const char*>(m_data.halfs.data());
            case PixelType::Float:    return reinterpret_cast<const char*>(m_data.floats.data());
            case PixelType::Double:   return reinterpret_cast<const char*>(m_data.doubles.data());
            default:                  throw std::runtime_error("Unsupported pixel type");
            }
        }

        PixelDataContainer& data() { return m_data; }

        void clear() { m_data.clear(); }

    private:
        std::string m_name{};
        PixelType m_pixelType = PixelType::Unknown;
        PixelDataContainer m_data;
    };

    /// @brief Stores per-layer channel buffers: layer name -> list of ChannelBuffer (e.g. R, G, B, A)
    struct LayerChannelBufferMap
    {
        std::map<std::string, std::vector<ChannelBuffer>> buffers;

        void set(const std::string& name, ChannelBuffer&& buf)
        {
            buffers[name].emplace_back(std::move(buf));
        }

        void set(const std::string& layerName, const std::string& channelName, PixelType pixelType, std::size_t size)
        {
            buffers[layerName].emplace_back(ChannelBuffer{channelName, pixelType, size});
        }

        bool hasLayer(const std::string& name)
        {
            return buffers.find(name) != buffers.end();
        }

        bool hasBuffer(const std::string& layerName, const std::string& bufferName)
        {
            auto it = buffers.find(layerName);
            if (it != buffers.end())
            {   
                auto ich = std::find_if(it->second.begin(), it->second.end(), 
                    [&](const auto& s){ return s.getName() == bufferName; });
                if (ich != it->second.end())
                    return true;
            }
            return false;
        }

        std::vector<ChannelBuffer>* getLayerBuffers(const std::string& name)
        {
            auto it = buffers.find(name);
            if (it != buffers.end())
                return &(it->second);
            else
                return nullptr;
        }

        ChannelBuffer* getBuffer(const std::string& layerName, const std::string& channelName)
        {
            auto it = buffers.find(layerName);
            if (it != buffers.end())
            {   
                auto ich = std::find_if(it->second.begin(), it->second.end(), 
                    [&](const auto& s){ return s.getName() == channelName; });
                if (ich != it->second.end())
                    return &(*ich);
            }
            return nullptr;
        }
    };
    
}

#endif
