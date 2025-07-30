#include <dmxdenoiser/ChannelInfo.hpp>
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

    std::string getFileExtension(std::string_view filename)
    {
        auto pos = filename.find_last_of('.');
        if (pos == std::string_view::npos)
            return {};
        std::string ext = std::string(filename.substr(pos + 1));
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });
        return ext;
    }

    ImageFileType getImageFileType(const std::string& filename)
    {
        auto ext = getFileExtension(filename);

        if (ext == "exr")   return ImageFileType::EXR;
        if (ext == "jpg")   return ImageFileType::JPG;
        if (ext == "jpeg")  return ImageFileType::JPEG;
        if (ext == "png")   return ImageFileType::PNG;

        return ImageFileType::Unknown;
    }
    
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

    void ExrImageIO::read(
            const std::string& filename,
            DMXImage& img,
            int frame,
            const AovDictionary& layers,
            int numChannels)
    {
        if (!img.hasValidLayerDictionary())
            throw std::runtime_error("The DMXImage does not have an initialized LayerDictionary");

        auto fileInfo = getImageInfo(filename);

        auto layerMap = fileInfo.layers;
        int width   = fileInfo.width;
        int height  = fileInfo.height;

        std::size_t buffSize = 
            static_cast<std::size_t>(width)*
            static_cast<std::size_t>(height);

        Imf::InputFile file(filename.data());
        Imath::Box2i dw = file.header().dataWindow();

        std::ptrdiff_t offset = dw.min.x + dw.min.y * width;

        Imf::FrameBuffer frameBuffer;

        LayerChannelBufferMap channelBuffers{};

        for(const auto& [layerName, exrLayerName] : layers)
        {
            if (!layerMap.hasLayer(exrLayerName))
                throw std::runtime_error("Layer " + exrLayerName + " does not exist.");
            
            auto channels = layerMap.getLayer(exrLayerName)->channels;

            for(int i = 0; i < numChannels; ++i)
            {
                if (i >= channels.size()) break;

                PixelType pixelType = channels[i].pixelType;
                std::string channelName = channels[i].name;
                img.getLayers().getLayer(layerName)->channels.emplace_back(channelName, pixelType); // Add channel info to DMXImage LayerDictionary

                std::string channelFullName = (exrLayerName == "default") ? channels[i].name : exrLayerName + "." + channelName;

                channelBuffers.set(layerName, channelName, pixelType, buffSize);
                char* base = channelBuffers.getBuffer(layerName, channelName)->getRawPtr();

                switch(pixelType)
                {
                    case PixelType::Float:
                    {
                        frameBuffer.insert(
                            channelFullName,
                            Imf::Slice(
                                Imf::FLOAT, 
                                base,
                                sizeof(float),
                                sizeof(float) * width,
                                1, 1,
                                0.0
                            )
                        );
                        break;
                    }
                    case PixelType::Half:
                    {
                        frameBuffer.insert(
                            channelFullName,
                            Imf::Slice(
                                Imf::HALF, 
                                base,
                                sizeof(half),
                                sizeof(half) * width,
                                1, 1,
                                0.0
                            )
                        );
                        break;
                    }
                    case PixelType::UInt32:
                    {
                        frameBuffer.insert(
                            channelFullName,
                            Imf::Slice(
                                Imf::UINT, 
                                base,
                                sizeof(unsigned int),
                                sizeof(unsigned int) * width,
                                1, 1,
                                0.0
                            )
                        );
                        break;
                    }
                    default: 
                        throw std::runtime_error("Channel " + channelFullName + " has unsupported pixel type.");
                }
            }
        }

        file.setFrameBuffer(frameBuffer);
        file.readPixels(dw.min.y, dw.max.y);

        for(const auto& [layer, buffers] : channelBuffers.buffers)
        {
            copyChannelBuffersToDMXImage(buffers, layer, frame, img);
        }
    }
    
    void ExrImageIO::write(
            const std::string& filename,
            const DMXImage& img,
            const std::vector<std::string>& layers) const
    {
        int width = img.width();
        int height = img.height();

        Imf::Header header (width, height);
        Imath::Box2i dataWindow{Imath::V2i(0, 0), Imath::V2i(width - 1, height - 1)};
        header.dataWindow() = dataWindow;

        auto buff = copyDMXImageToChannelBuffers(img, layers);

        for (const auto& channel : buff)
        {
            Imf::PixelType pixelType = toEXRPixelType(channel.getPixelType());
            header.channels().insert(channel.getName(), Imf::Channel(pixelType));
        }

        Imf::OutputFile file(filename.c_str(), header);
        Imf::FrameBuffer frameBuffer;

        for (int i = 0; i < buff.size(); ++i)
        {
            std::string channelName = buff[i].getName();
            Imf::PixelType pixelType = toEXRPixelType(buff[i].getPixelType());
            std::size_t pixelTypeSize = getPixelTypeSize(buff[i].getPixelType());
            char* buffBasePtr = buff[i].getRawPtr();

            frameBuffer.insert (
                channelName, // name
                Imf::Slice (
                    pixelType,                        
                    buffBasePtr,
                    pixelTypeSize * 1,
                    pixelTypeSize * width));
        }
        
        file.setFrameBuffer (frameBuffer);
        file.writePixels (dataWindow.max.y - dataWindow.min.y + 1);
    }
    
    ImageInfo ExrImageIO::getImageInfo(const std::string& filename) const
    {
        
        ImageInfo info{};
        info.type = ImageFileType::EXR;

        Imf::InputFile file(filename.data());
        Imath::Box2i dw = file.header().dataWindow();

        info.width = dw.max.x - dw.min.x + 1;
        info.height = dw.max.y - dw.min.y + 1;

        info.params.addInt("compression", static_cast<int>(file.header().compression()));

        const Imf::ChannelList &channels = file.header().channels();

        for (Imf::ChannelList::ConstIterator i = channels.begin(); i != channels.end(); ++i) 
        {
            std::string name = i.name();
            std::string layer{};
            std::string channel{};
            auto pos = name.rfind('.');
            if (pos != std::string::npos)
            {
                layer = name.substr(0, pos);
                channel = name.substr(pos + 1);
            } else
            {
                layer = "default";
                channel = name;
            }
            info.layers.addLayer(layer);
            info.layers.getLayer(layer)->addChannel(channel, toPixelType(i.channel().type));
        }
        
        return info;
    }
    
    ExrImageIO::~ExrImageIO(){}

} // namespace dmxdenoiser
