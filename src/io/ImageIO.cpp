#include <algorithm>
#include <dmxdenoiser/io/ImageIO.hpp>
#include <dmxdenoiser/image/DMXImage.hpp>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfArray.h>
#include <Imath/half.h>
#include <Imath/ImathBox.h>
#include <cstddef> // for std::ptrdiff_t and std::size_t

namespace dmxdenoiser::io
{

std::string toLower(const std::string& s)
{
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c){ return std::tolower(c); });
    return result;
}

int ExrImageInfo::channelCount() const 
{ 
    int count = 0; 
    for (const auto& l : layers)
        count += l.channels.size();
    return count;
};

int ExrIOParams::channelCount() const
{
    int count = 0; 
    for (const auto& l: layers) 
        count += l.channels.size();
    return count;
}

bool ExrImageIO::read(
        std::string_view filename,
        float* img,
        const ImageIOParams* params)
{
    using namespace Settings;

    auto exrParams = dynamic_cast<const ExrIOParams*>(params);

    Imf::InputFile file(filename.data());

    Imath::Box2i dw = file.header().dataWindow();
    int width   = dw.max.x - dw.min.x + 1;
    int height  = dw.max.y - dw.min.y + 1;
    
    Imf::FrameBuffer frameBuffer;

    std::vector<DMXLayer> tmpBuffers;

    // RGBA
    for (const auto& layer : exrParams->layers)
    {
        tmpBuffers.emplace_back(layer.name);
        for (const auto& c : layer.channels)
        {
            tmpBuffers.emplace_back(c.name);
            std::string name = layer.name + "." + c.name;
            std::ptrdiff_t offset = dw.min.x * layer.channels.size() + dw.min.y * width * layer.channels.size();

            switch(c.pixelType.value())
            {
            case Imf::FLOAT: 
            {
                float* buf = new float[width * height * layer.channels.size()];
                tmpBuffers.back().channels.emplace_back(name, buf);
                tmpBuffers.back().channels.back().metadata = "FLOAT:" + std::to_string(layer.channels.size());
                frameBuffer.insert(
                    name, /*channel name*/
                    Imf::Slice(
                        Imf::FLOAT, 
                        reinterpret_cast<char*>(buf - offset),
                        sizeof(float) * layer.channels.size(),
                        sizeof(float) * width * layer.channels.size()
                    )
                );
                break;
            }
            case Imf::HALF:
            {
                half* buf = new half[width * height * layer.channels.size()];
                tmpBuffers.back().channels.emplace_back(name, buf);
                tmpBuffers.back().channels.back().metadata = "HALF:" + std::to_string(layer.channels.size());
                frameBuffer.insert(
                    name, /*channel name*/
                    Imf::Slice(
                        Imf::HALF, 
                        reinterpret_cast<char*>(buf - offset),
                        sizeof(half) * layer.channels.size(),
                        sizeof(half) * width * layer.channels.size()
                    )
                );
                break;
            }
            case Imf::UINT:
            {
                unsigned int* buf = new unsigned int[width * height * layer.channels.size()];
                tmpBuffers.back().channels.emplace_back(name, buf);
                tmpBuffers.back().channels.back().metadata = "UINT:" + std::to_string(layer.channels.size());
                frameBuffer.insert(
                    name, /*channel name*/
                    Imf::Slice(
                        Imf::UINT, 
                        reinterpret_cast<char*>(buf - offset),
                        sizeof(unsigned int) * layer.channels.size(),
                        sizeof(unsigned int) * width * layer.channels.size()
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

    for(int w = 0; w < width; ++w)
        for(int h = 0; h < height; ++w)
        {
            for (const auto& layer : exrParams->layers)
            {
                int offset = width * height * layer.channels.size();
            }
            
        }

    // Delete tmpBuffers
    for(auto& l : tmpBuffers)
        for(auto& chan : l.channels)
        {
            if (chan.metadata == "FLOAT")
                delete[] reinterpret_cast<float*>(chan.ptr);
            if (chan.metadata == "HALF")
                delete[] reinterpret_cast<half*>(chan.ptr);
            if (chan.metadata == "UINT")
                delete[] reinterpret_cast<unsigned int*>(chan.ptr);
        }
    
    return true;
}

bool ExrImageIO::write(
        std::string_view filename,
        const float* img,
        const ImageIOParams* params) const
{
    return true; // pass
}

std::unique_ptr<ImageInfo> ExrImageIO::getImageInfo(std::string_view filename) const
{
    auto info = std::make_unique<ExrImageInfo>();
    Imf::InputFile file(filename.data());
    Imath::Box2i dw = file.header().dataWindow();

    info->width = dw.max.x - dw.min.x + 1;
    info->height = dw.max.y - dw.min.y + 1;

    info->compression = static_cast<ExrCompression>(file.header().compression());

    const Imf::ChannelList &channels = file.header().channels();

    //std::set<std::string> layerSet;
    std::string defaultChannelSet{ "rgba" };
    info->layers.clear();

    std::map<std::string, std::vector<ExrChannel>> layersMap{};
    for (Imf::ChannelList::ConstIterator i = channels.begin(); i != channels.end(); ++i) 
    {
        std::string name = i.name();
        auto pos = name.rfind('.');
        std::string layer{};
        std::string channel{};
        if (pos != std::string::npos)
        {
            layer = name.substr(0, pos);
            channel = name.substr(pos + 1);
        } else
        {
            channel = name;
            auto it = defaultChannelSet.find(channel.empty() ? '\0' : std::tolower(channel[0]));
            if (it != std::string::npos)
                layer = "default";
            else
                layer = "others";
        }

        layersMap[layer].emplace_back(channel, i.channel().type);
    }

    // Sort channels
    std::string order{ "rgbaz" };
    for (auto& [layer, channel] : layersMap)
    {
        std::sort(channel.begin(), channel.end(), 
            [&](const auto& a, const auto& b){
                auto ia = order.find(a.name.empty() ? '\0' : std::tolower(a.name[0]));
                auto ib = order.find(b.name.empty() ? '\0' : std::tolower(b.name[0]));

                if (ia == std::string::npos)
                    ia = order.size();
                if (ib == std::string::npos)
                    ib = order.size();
                return ia < ib;
        });
        
    }
    for(const auto& [layer, channels] : layersMap)
    {
        info->layers.emplace_back(layer);
        for(const auto& ch : channels)
        {
            info->layers.back().channels.emplace_back(ch.name, ch.type);
        }
    }
    //params->layers.assign(layerSet.begin(), layerSet.end());
    return info; // pass
}

ExrImageIO::~ExrImageIO(){}

}
