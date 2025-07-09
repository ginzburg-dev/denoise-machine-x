#include <algorithm>
#include <dmxdenoiser/io/ImageIO.hpp>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfArray.h>
#include <Imath/half.h>
#include <Imath/ImathBox.h>

namespace dmxdenoiser::io
{

std::string toLower(const std::string& s)
{
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c){ return std::tolower(c); });
    return result;
}

bool ExrImageIO::read(
        std::string_view filename,
        float* img,
        const ImageIOParams* params)
{
    int c = Settings::numChannels;
    auto exrParams = dynamic_cast<const ExrIOParams*>(params);

    Imf::InputFile file(filename.data());

    Imath::Box2i dw = file.header().dataWindow();
    int width   = dw.max.x - dw.min.x + 1;
    int height  = dw.max.y - dw.min.y + 1;
    
    Imf::FrameBuffer frameBuffer;

    // RGBA
    for (const auto& i : exrParams->channels)
    {
        if (toLower(i.name) == "r")
        {
            std::vector<half> pixels{};
        //pixels.resize(width * height * Settings::numChannels);
            frameBuffer.insert (
            i.name.data(),
            Imf::Slice (
                i.type,
                (char*)(&pixels[0] - dw.min.x*4 - dw.min.y*width*4),
                sizeof (pixels[0]) * c,
                sizeof (pixels[0]) * width * c));
        }

        //std::vector<half> pixels{};
        //pixels.resize(width * height * Settings::numChannels);
    }
    /*
    {
        i.channel().type
        const char* typeStr = nullptr;
        switch (i.channel().type) 
        {
        case Imf::UINT:  typeStr = "UINT"; break;
        case Imf::HALF:  typeStr = "HALF"; break;
        case Imf::FLOAT: typeStr = "FLOAT"; break;
        default:         typeStr = "UNKNOWN"; break;
        }
        std::cout << i.name() << ' ' << typeStr << '\n';
    }
    constexpr std::array<std::string_view, 4> c = { "R", "G", "B", "A" };

    for (int i = 0; i < 4; ++i)
        frameBuffer.insert (
            c[i].data(),
            Imf::Slice (
                Imf::HALF,
                (char*)(pixels.data() + i - dw.min.x*4 - dw.min.y*width*4),
                sizeof (pixels[0]) * 4,
                sizeof (pixels[0]) * width * 4));
    
    fileInput.setFrameBuffer(frameBuffer);
    fileInput.readPixels(dw.min.y, dw.max.y);
    */
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
    std::set<std::string> layerSet;
    info->channels.clear();
    //info->layers.clear();
    for (Imf::ChannelList::ConstIterator i = channels.begin(); i != channels.end(); ++i) 
    {
        std::string name = i.name();
        info->channels.emplace_back(i.name(), i.channel().type);
        /*
        auto pos = name.find('.');
        if (pos != std::string::npos)
            layerSet.insert(name.substr(0, pos));
        */
    }
    //params->layers.assign(layerSet.begin(), layerSet.end());
    return info; // pass
}

ExrImageIO::~ExrImageIO(){}

}
