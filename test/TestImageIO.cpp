#include <dmxdenoiser/io/ImageIO.hpp>
#include <memory>
#include <array>
#include <iostream>
#include <cassert>
#include <string>
#include <string_view>

using namespace dmxdenoiser::io;

int main()
{
    std::cout << "ImageIO Test\n";

    auto IO = new ExrImageIO();
    auto info = IO->getImageInfo("../examples/exr_samples/sample2x2.exr");
    auto exrInfo = dynamic_cast<ExrImageInfo*>(info.get());

    std::vector<std::string> channels2x2 
        { "Diffuse.red", "Diffuse.green", "Diffuse.blue", "default.R", "default.G", "default.B", "default.A", "depth.Z" };

    std::cout << "Channels: \n";
    int i = 0;
    for (const auto& layer : exrInfo->layers)
    {
        std::cout << "Layer: " << layer.name << " {";
        
        for (const auto& c : layer.channels)
        {
            std::cout << "Comparing: " << (layer.name + "." + c.name) << " <-> " << channels2x2[i] << '\n';
            assert((layer.name + "." + c.name) == channels2x2[i]);
            assert(c.pixelType == Imf::PixelType::HALF);
            std::cout << "{" << c.name << ", " << c.pixelType.value() << "} ";
            ++i;
        }
        std::cout << "}\n";
        
    }
    
    std::cout << "Channel count in Info: " << exrInfo->channelCount() << '\n';
    assert(exrInfo->channelCount() == channels2x2.size());

    auto param = std::make_unique<ExrIOParams>();
    for (const auto& layer : exrInfo->layers)
    {
        param->layers.emplace_back(layer.name);
        param->layers.back().channels = layer.channels;
    }
    std::cout << "Channel count in Params: " << param->channelCount() << '\n';
    assert(param->channelCount() == channels2x2.size());



    std::cout << '\n';

    std::cout << "Compression : " << static_cast<int>(exrInfo->compression) << '\n';
    std::cout << "Width : " << exrInfo->width << '\n';
    std::cout << "Height: " << exrInfo->height << '\n';

    assert(exrInfo->width == 2);
    assert(exrInfo->height == 2);
    assert(exrInfo->compression == ExrCompression::ZIPS);


    info = IO->getImageInfo("../test/sample2x2_write.exr");
    exrInfo = dynamic_cast<ExrImageInfo*>(info.get());

    for (const auto& layer : exrInfo->layers)
    {
        std::cout << "Layer: " << layer.name << " {";
        
        for (const auto& c : layer.channels)
        {
            //std::cout << "Comparing: " << (layer + "." + c.name) << " <-> " << channels2x2[i] << '\n';
            //assert((layer + "." + c.name) == channels2x2[i]);
            //assert(c.type == Imf::PixelType::HALF);
            std::cout << "{" << c.name << ", " << c.pixelType.value() << "} ";
            ++i;
        }
        std::cout << "}\n";
        
    }


    delete IO;
    
    return 0;
}
