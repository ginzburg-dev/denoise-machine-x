#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/ImageIO.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/PixelType.hpp>
#include <dmxdenoiser/StringConversions.hpp>

#include <algorithm>
#include <cstddef>
#include <unordered_map>

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
    
    bool ExrImageIO::read(
            const std::string& filename,
            DMXImage& img,
            const AovDictionary& layers)
    {
        /*
        auto exrParams = dynamic_cast<const ExrIOParams*>(params);

        Imf::InputFile file(filename.data());

        Imath::Box2i dw = file.header().dataWindow();
        int width   = dw.max.x - dw.min.x + 1;
        int height  = dw.max.y - dw.min.y + 1;

        Imf::FrameBuffer frameBuffer;

        std::vector<LayerInfo> tmpBuffers;
        LayerChannelBufferMap channelBuffers{};


        for (const auto& layer : exrParams->layers)
        {
            //tmpBuffers.emplace_back(layer.name);

            for (const auto& c : layer.channels)
            {
                std::string name{};
                if ( layer.name == "default" ||  layer.name == "others" )
                    name = c.name;
                else
                    name = layer.name + "." + c.name;


                std::ptrdiff_t offset = dw.min.x + dw.min.y * width;

                //tmpBuffers.back().channels.emplace_back(name, c.pixelType);
                channelBuffers[layer.name].push_back(ChannelBuffer::createEmpty(name, c.pixelType, width * height));
                switch(c.pixelType) !!!! NEED TO BE COMPARED AS A SOURCE TYPE !!!!
                {
                case Imf::FLOAT: !!!! NEED TO BE COMPARED AS A SOURCE TYPE !!!!
                {
                    float* buf = new float[width * height];
                    tmpBuffers.back().channels.back().ptr = buf;
                    frameBuffer.insert(
                        name,
                        Imf::Slice(
                            Imf::FLOAT, 
                            reinterpret_cast<char*>(buf - offset),
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
                    half* buf = new half[width * height];
                    tmpBuffers.back().channels.back().ptr = buf;
                    frameBuffer.insert(
                        name,
                        Imf::Slice(
                            Imf::HALF, 
                            reinterpret_cast<char*>(buf - offset),
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
                    unsigned int* buf = new unsigned int[width * height];
                    tmpBuffers.back().channels.back().ptr = buf;
                    frameBuffer.insert(
                        name,
                        Imf::Slice(
                            Imf::UINT, 
                            reinterpret_cast<char*>(buf - offset),
                            sizeof(unsigned int),
                            sizeof(unsigned int) * width,
                            1, 1,
                            0.0
                        )
                    );
                    break;
                }
                default:
                {
                    throw std::runtime_error("ReadExr[Error]: Channel " + name + " has unknown pixel type.");
                    break;
                }
                }
            }

        }
        file.setFrameBuffer(frameBuffer);
        file.readPixels(dw.min.y, dw.max.y);

        int offset = width * height * NUM_CHANNELS;
        for(int w = 0; w < width; ++w)
            for(int h = 0; h < height; ++h)
            {
                for (int l = 0; l < exrParams->layers.size(); ++l)
                {
                    auto& layer = exrParams->layers[l];
                    DMXLayer* tmpLayer = nullptr;
                    auto it = std::find_if(tmpBuffers.begin(), tmpBuffers.end(), [&](const auto& tmpLayer){ return layer.name == tmpLayer.name; });
                    if (it != tmpBuffers.end())
                        tmpLayer = &(*it);
                    else
                        throw std::runtime_error("ImageIO.cpp:143[Error]: Channel name mismatch.");

                    for(int c = 0; c < NUM_CHANNELS; ++c)
                    {
                        auto* basePixel = reinterpret_cast<float*>(layer.ptr);
                        auto* pixel = basePixel + (h*width + w) * NUM_CHANNELS + c;
                    
                        if(c < layer.channels.size())
                        {
                            auto type = tmpLayer->channels[c].pixelType;
                            if(!type.has_value())
                                throw std::runtime_error("[Error]: Pixel Type was not provided.");
                        
                            if(type == Imf::FLOAT)
                            {
                                auto* baseTmpPixel = reinterpret_cast<float*>(tmpLayer->channels[c].ptr);
                                auto* tmpPixel = baseTmpPixel + (h*width + w);
                                *pixel = *tmpPixel;
                            }

                            if(type == Imf::HALF)
                            {
                                auto* baseTmpPixel = reinterpret_cast<half*>(tmpLayer->channels[c].ptr);
                                auto* tmpPixel = baseTmpPixel + (h*width + w);
                                *pixel = *tmpPixel;
                            }

                            if(type == Imf::UINT)
                            {
                                auto* baseTmpPixel = reinterpret_cast<unsigned int*>(tmpLayer->channels[c].ptr);
                                auto* tmpPixel = baseTmpPixel + (h*width + w);
                                *pixel = *tmpPixel;
                            }

                        }
                        else
                        {
                            *pixel = 0.0f; 
                        }
                    }
                }

            }

        // Delete tmpBuffers
        for(auto& l : tmpBuffers)
            for(auto& chan : l.channels)
            {
                Imf::PixelType type{};
                if (chan.pixelType.has_value())
                    type = chan.pixelType.value();
                else
                    throw std::runtime_error("InageIO.cpp:151[Error]: Wrong pixelType metadata.");
                
                if (type == Imf::FLOAT)
                    delete[] reinterpret_cast<float*>(chan.ptr);
                if (type == Imf::HALF)
                    delete[] reinterpret_cast<half*>(chan.ptr);
                if (type == Imf::UINT)
                    delete[] reinterpret_cast<unsigned int*>(chan.ptr);
            }
        */
        return true;
    }
    
    bool ExrImageIO::write(
            const std::string& filename,
            DMXImage& img,
            const AovDictionary& layers) const
    {
        return true; // pass
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
