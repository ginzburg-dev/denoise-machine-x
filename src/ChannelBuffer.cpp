#include <dmxdenoiser/ChannelBuffer.hpp>
#include <dmxdenoiser/PixelType.hpp>

#include <algorithm>
#include <cstddef>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include <Imath/half.h>

namespace dmxdenoiser
{

    void PixelDataContainer::allocate(PixelType type, std::size_t size)
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
    void PixelDataContainer::clear() { 
        std::fill(uint8s.begin(), uint8s.end(), 0);
        std::fill(uint16s.begin(), uint16s.end(), 0);
        std::fill(uint32s.begin(), uint32s.end(), 0);
        std::fill(halfs.begin(), halfs.end(), half(0.0f));
        std::fill(floats.begin(), floats.end(), 0.0f);
        std::fill(doubles.begin(), doubles.end(), 0.0);
    }
    PixelDataContainer::PixelDataContainer(PixelType type, std::size_t size)
    {
        allocate(type, size);
    }
    
    ChannelBuffer::ChannelBuffer(const std::string& name, PixelType pixelType, std::size_t size)
        : m_name{name}, m_pixelType{pixelType}, m_data{PixelDataContainer{pixelType, size}} 
    {}

    void ChannelBuffer::initialize(const std::string& name, PixelType pixelType, std::size_t size)
    {
        m_name = name;
        m_pixelType = pixelType;
        m_data.allocate(pixelType, size);
    }

    char* ChannelBuffer::getRawPtr()
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

    const char* ChannelBuffer::getRawPtr() const
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

    void LayerChannelBufferMap::set(const std::string& name, ChannelBuffer&& buf)
    {
        buffers[name].emplace_back(std::move(buf));
    }

    void LayerChannelBufferMap::set(const std::string& layerName, const std::string& channelName, PixelType pixelType, std::size_t size)
    {
        buffers[layerName].emplace_back(ChannelBuffer{channelName, pixelType, size});
    }

    bool LayerChannelBufferMap::hasLayer(const std::string& name)
    {
        return buffers.find(name) != buffers.end();
    }

    bool LayerChannelBufferMap::hasBuffer(const std::string& layerName, const std::string& bufferName)
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

    std::vector<ChannelBuffer>* LayerChannelBufferMap::getLayerBuffers(const std::string& name)
    {
        auto it = buffers.find(name);
        if (it != buffers.end())
            return &(it->second);
        else
            return nullptr;
    }

    ChannelBuffer* LayerChannelBufferMap::getBuffer(const std::string& layerName, const std::string& channelName)
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
    
} // namespace dmxdenoiser
