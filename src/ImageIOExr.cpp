#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/ChannelBuffer.hpp>
#include <dmxdenoiser/ImageFileType.hpp>
#include <dmxdenoiser/ImageIOExr.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/PixelType.hpp>
#include <dmxdenoiser/StringConversions.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <sstream>
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
    std::unique_ptr<ImageIOExr> ImageIOExr::create()
    {
        return std::make_unique<ImageIOExr>();
    }

    void ImageIOExr::read(
            const std::string& filename,
            DMXImage& img,
            int frame,
            const AovDictionary& layers)
    {
        if(!std::filesystem::exists(filename)) {
            DMX_LOG_ERROR("ImageIOExr", "read(): File \"", filename, "\" not found.");
            throw std::runtime_error("File \"" + filename + "\" not found.");
        }

        int numChannels = DEFAULT_NUM_CHANNELS;
        
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
    
    void ImageIOExr::write(
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
    
    ImageInfo ImageIOExr::getImageInfo(const std::string& filename) const
    {
        if(!std::filesystem::exists(filename)) {
            DMX_LOG_ERROR("ImageIOExr", "getImageInfo(): File \"", filename, "\" not found.");
            throw std::runtime_error("File \"" + filename + "\" not found.");
        }

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
            info.layers.getLayer(layer)->originalName = layer;
            info.layers.getLayer(layer)->addChannel(channel, toPixelType(i.channel().type));
        }
        
        return info;
    }

} // namespace dmxdenoiser
