#include <dmxdenoiser/image/DMXImage.hpp>
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
            std::cout << "{" << c.name << ", " << c.pixelType.value() << "} ";
            ++i;
        }
        std::cout << "}\n";
        
    }


    /// 
    {
        auto IO = std::make_unique<ExrImageIO>();
        auto info = IO->getImageInfo("../examples/exr_samples/sample2x2_diff.exr");
        auto exrInfo = dynamic_cast<ExrImageInfo*>(info.get());

        for (const auto& layer : exrInfo->layers)
        {
            std::cout << "Layer: " << layer.name << " {";

            for (const auto& c : layer.channels)
            {
                std::cout << "{" << c.name << ", " << c.pixelType.value() << "} ";
                ++i;
            }
        }
        std::cout << "}\n";

        auto paramDiff = std::make_unique<ExrIOParams>();
        int c = dmxdenoiser::Settings::numChannels;
        int width = exrInfo->width;
        int height = exrInfo->height;
        int layersCount = 3;
        int offset = width * height * c;

        dmxdenoiser::DMXImage image{width, height, {"default", "Diffuse", "depth"}, 1};

        std::vector<float> data(width * height * layersCount * c);
        std::fill(data.begin(), data.end(), 0);

        auto* base = data.data();

        paramDiff.get()->layers.emplace_back("default", base + offset*0);
        paramDiff.get()->layers.back().channels.emplace_back("R", Imf::HALF);
        paramDiff.get()->layers.back().channels.emplace_back("G", Imf::HALF);
        paramDiff.get()->layers.back().channels.emplace_back("B", Imf::HALF);
        paramDiff.get()->layers.back().channels.emplace_back("A", Imf::HALF);

        paramDiff.get()->layers.emplace_back("Diffuse", base + offset*1);
        paramDiff.get()->layers.back().channels.emplace_back("R", Imf::HALF);
        paramDiff.get()->layers.back().channels.emplace_back("G", Imf::HALF);
        paramDiff.get()->layers.back().channels.emplace_back("B", Imf::HALF);
        paramDiff.get()->layers.back().channels.emplace_back("A", Imf::HALF);

        paramDiff.get()->layers.emplace_back("depth", base + offset*2);
        paramDiff.get()->layers.back().channels.emplace_back("R", Imf::HALF);
        paramDiff.get()->layers.back().channels.emplace_back("G", Imf::HALF);
        paramDiff.get()->layers.back().channels.emplace_back("B", Imf::HALF);
        paramDiff.get()->layers.back().channels.emplace_back("A", Imf::HALF);

        try
        {
            IO->read("../examples/exr_samples/sample2x2_mono.exr", nullptr, paramDiff.get());
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        auto info_mono = IO->getImageInfo("../examples/exr_samples/sample2x2_mono.exr");
        auto exr_info_mono = dynamic_cast<ExrImageInfo*>(info_mono.get());

        for (const auto& layer : exr_info_mono->layers)
        {
            std::cout << "Layer: " << layer.name << " {";

            for (const auto& c : layer.channels)
            {
                std::cout << "{" << c.name << ", " << c.pixelType.value() << "} ";
                ++i;
            }
        }
        const float comparingTable[] = {
            1.000000f, 1.000000f, 1.000000f, 1.000000f, 1.000000f, 
            1.000000f, 1.000000f, 1.000000f, 1.000000f, 1.000000f, 
            1.000000f, 1.000000f, 1.000000f, 1.000000f, 1.000000f, 
            1.000000f, 0.000000f, 0.000000f, 0.000000f, 1.000000f, 
            0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f, 
            0.000000f, 0.000000f, 1.000000f, 0.000000f, 0.000000f, 
            0.000000f, 1.000000f, 0.556641f, 0.556641f, 0.556641f, 
            1.000000f, 0.556641f, 0.556641f, 0.556641f, 1.000000f, 
            0.556641f, 0.556641f, 0.556641f, 1.000000f, 0.556641f, 
            0.556641f, 0.556641f, 1.000000f
        };
        std::cout << "Pixels: \n";
        for (int i = 0; i < data.size(); ++i)
        {
            //std::cout << data[i*4] << ", " <<  data[i*4 + 1] << ", " << data[i*4 + 2] << ", " << data[i*4 + 3] << ", ";
            std::cout << data[i] << "f, ";
            //assert(comparingTable[i] == data[i]);
        }

        for(int l = 0; l < layersCount; ++l)
        {
            std::cout << "Layer: " << paramDiff.get()->layers[l].name << '\n';
            for(int h = 0; h < height; ++h)
            {
                for(int w = 0; w < width; ++w)
                {
                    std::cout << "<" << data[(l*width*height + (width*h + w))*4] << ", " << data[(l*width*height + (width*h + w))*4 + 1] << ", " << data[(l*width*height + (width*h + w))*4 + 2] << ", " << data[(l*width*height + (width*h + w))*4 + 3] << ">   ";
                }
                std::cout << '\n';
            }
        }
        
        
    }
    delete IO;
    
    return 0;
}
