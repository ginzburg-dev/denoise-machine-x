#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/ImageFileType.hpp>
#include <dmxdenoiser/ImageIO.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/PixelType.hpp>
#include <dmxdenoiser/StringConversions.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <unordered_map>
#include <utility>

#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfPixelType.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfArray.h>
#include <Imath/half.h>
#include <Imath/ImathBox.h>

namespace dmxdenoiser
{
    
    std::string ImageInfo::ToString() const
    {
        std::ostringstream oss;
        oss << "ImageInfo: \n";
        oss << "    Dimensions: " << width << " x " << height << '\n';
        oss << "    ImageFileType: " << type << '\n';
        oss << "    Layers: ";
        for (const auto& i : layers.data())
        {
            oss << i.first << ' ';
        }
        return oss.str();
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
