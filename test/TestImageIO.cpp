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

    constexpr std::array<std::string_view, 8> channels2x2 
        { "A", "B", "Diffuse.blue", "Diffuse.green", "Diffuse.red", "G", "R", "depth.Z" };

    std::cout << "Channels: \n";
    for (int i = 0; i < exrInfo->channels.size(); ++i)
    {
        std::cout << exrInfo->channels[i].name << ' ';
        assert(exrInfo->channels[i].name == channels2x2[i]);
        assert(exrInfo->channels[i].type == Imf::PixelType::HALF);
    }
        
    std::cout << '\n';

    std::cout << "Compression : " << static_cast<int>(exrInfo->compression) << '\n';
    std::cout << "Width : " << exrInfo->width << '\n';
    std::cout << "Height: " << exrInfo->height << '\n';

    assert(exrInfo->width == 2);
    assert(exrInfo->height == 2);
    assert(exrInfo->compression == ExrCompression::ZIPS);

    delete IO;
    
    return 0;
}
