#include <dmxdenoiser/ChannelBuffer.hpp>
#include <dmxdenoiser/DMXImage.hpp>
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

    void copyChannelBuffersToDMXImage(
        const std::vector<ChannelBuffer>& buff, 
        std::string_view layer, 
        int frame, 
        DMXImage& img)
    {
        for(int c = 0; c < buff.size(); ++c)
        {
            std::string channelName = buff[c].getName();
            std::size_t baseIndex = img.getPixelIndex(0, 0, frame, layer);
            PixelType pixelType = buff[c].getPixelType();
            const char* buffRawPtr = buff[c].getRawPtr();
            auto& pixels = img.data();
            
            for(std::size_t i = 0; i < img.width() * img.height(); ++i)
            {
                std::size_t index = baseIndex + i * img.numChannels() + c;
                
                switch(pixelType)
                {
                    case PixelType::UInt8: {
                        pixels[index] = static_cast<float>(reinterpret_cast<const uint8_t*>(buffRawPtr)[i]);
                        break;
                    }
                    case PixelType::UInt16: {
                        pixels[index] = static_cast<float>(reinterpret_cast<const uint16_t*>(buffRawPtr)[i]);
                        break;
                    }
                    case PixelType::UInt32: {
                        pixels[index] = static_cast<float>(reinterpret_cast<const uint32_t*>(buffRawPtr)[i]);
                        break;
                    }
                    case PixelType::Half: {
                        pixels[index] = static_cast<float>(reinterpret_cast<const half*>(buffRawPtr)[i]);
                        break;
                    }
                    case PixelType::Float: {
                        pixels[index] = static_cast<float>(reinterpret_cast<const float*>(buffRawPtr)[i]);
                        break;
                    }
                    case PixelType::Double: {
                        pixels[index] = static_cast<float>(reinterpret_cast<const double*>(buffRawPtr)[i]);
                        break;
                    }
                    default: throw std::runtime_error("Unsupported pixel type");
                }
            }
        }
    }

    std::vector<ChannelBuffer> copyDMXImageToChannelBuffers(
        const DMXImage& img,
        const std::vector<std::string>& layers
        )
    {
        std::vector<ChannelBuffer> buff;
        buff.resize(0);

        int width = img.width();
        int height = img.height();
        int bufferSize = width * height;
        auto& pixels = img.data();

        for(int i = 0; i < layers.size(); ++i)
        {
            if(!img.hasLayer(layers[i]))
                throw std::runtime_error("DMXImage does not contain \"" + layers[i] + "\" layer");

            auto layerInfo = img.getLayers().getLayer(layers[i]);
            std::size_t layerBaseIndex = img.getPixelIndex(0, 0, 0, layers[i]);
            auto layerExrName = img.getLayers().getLayer(layers[i])->originalName;
            auto layerChannels = img.getLayers().getLayer(layers[i])->channels;
            int numChannels = layerChannels.size();
            int numImageChannels = img.numChannels();

            for (int c = 0; c < numChannels; ++c)
            {
                std::string channelName = 
                    (layerExrName == "default") ? layerChannels[c].name : layerExrName + "." + layerChannels[c].name;
                PixelType pixelType = layerChannels[c].pixelType;
                buff.emplace_back(channelName, pixelType, bufferSize);
                char* bufRawPtr = buff.back().getRawPtr();

                for(int b = 0; b < bufferSize; ++b)
                {
                    std::size_t index = layerBaseIndex + b * numImageChannels + c;

                    switch(pixelType)
                    {
                        case PixelType::UInt8: {
                            reinterpret_cast<uint8_t*>(bufRawPtr)[b] = static_cast<uint8_t>(pixels[index]);
                            break;
                        }
                        case PixelType::UInt16: {
                            reinterpret_cast<uint16_t*>(bufRawPtr)[b] = static_cast<uint16_t>(pixels[index]);
                            break;
                        }
                        case PixelType::UInt32 : {
                            reinterpret_cast<uint32_t*>(bufRawPtr)[b] = static_cast<uint32_t>(pixels[index]);
                            break;
                        }
                        case PixelType::Half: {
                            reinterpret_cast<half*>(bufRawPtr)[b] = static_cast<half>(pixels[index]);
                            break;
                        }
                        case PixelType::Float: {
                            reinterpret_cast<float*>(bufRawPtr)[b] = pixels[index];
                            break;
                        }
                        case PixelType::Double: {
                            reinterpret_cast<double*>(bufRawPtr)[b] = static_cast<double>(pixels[index]);
                            break;
                        }
                        default: throw std::runtime_error("Unsupported pixel type");
                    }
                }
            }  
        }
        return buff;
    }
    
} // namespace dmxdenoiser
