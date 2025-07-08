#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfHeader.h>
#include <Imath/half.h>
#include <Imath/ImathBox.h>
#include <iostream>
#include <string>
#include <set>
/*
struct Rgba
{
    half r;
    half g;
    half b;
    half a;
};
*/
int main()
{
    std::cout << "OpenEXR lib test\n";
    std::cout << "OpenEXR version: " << OPENEXR_VERSION_STRING << '\n';    

    Imf::RgbaInputFile file("../examples/exr_samples/2x2sample.exr");

    const Imf::ChannelList &channels = file.header().channels();

    for (Imf::ChannelList::ConstIterator i = channels.begin(); i != channels.end(); ++i)
        std::cout << *i.name() << '\n';

    std::set<std::string> layerNames;
    for (Imf::ChannelList::ConstIterator i = channels.begin(); i != channels.end(); ++i) {
        std::string name = i.name();
        std::cout << i.name() << '\n';

        auto pos = name.find('.');
        if (pos != std::string::npos)
            layerNames.insert(name.substr(0, pos));
    }

    //channels.layers(layerNames);
/*
    for (auto i = layerNames.begin(); i != layerNames.end(); ++i)
        std::cout << "EXR Layer: " << *i << '\n'; 
*/


    /*WRITE RGBA 1*/
    
    int width = 2;
    int height = 2;
    Imf::Rgba* pixels = new Imf::Rgba[width * height];

    Imf::RgbaOutputFile outFile("../test/2x2sample_write.exr", width, height, Imf::WRITE_RGBA);
    outFile.setFrameBuffer(pixels, 1, width);
    outFile.writePixels(height);

    delete[] pixels;

    return 0;
}
