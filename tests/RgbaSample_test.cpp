#include <dmxdenoiser/ImageIOExr.hpp>
#include <dmxdenoiser/PixelType.hpp>

#include <iostream>
#include <filesystem>
#include <memory>

#include <OpenEXR/ImfCompression.h>
#include <OpenEXR/ImfNamespace.h>

using namespace dmxdenoiser;

int main()
{
    if (!std::filesystem::exists("../tests/test_files/sample_channels/rgba.0001.exr"))
        return 0;

    auto io = ImageIOExr::create();
    auto info = io->getImageInfo("../tests/test_files/sample_channels/rgba.0001.exr");
    std::cout << info.ToString() << '\n';
    auto compression = info.params.getSingleParam<int>("compression");
    if(compression)
        std::cout << "    Compresion: " << exrCompressionName(static_cast<Imf::Compression>(*compression)) << '\n';
    std::cout << "    Channels: \n";
    for (auto& [layer, layerInfo] : info.layers.data())
        for (auto& c : layerInfo.channels)
            std::cout << "      " << layerInfo.originalName << "." << c.name << ": " << ToString(c.pixelType) << '\n';
    return 0;
}
