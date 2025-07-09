#include <dmxdenoiser/io/ImageIO.hpp>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfArray.h>
#include <Imath/half.h>
#include <Imath/ImathBox.h>
#include <iostream>
#include <string>
#include <string_view>
#include <set>
#include <vector>
#include <array>
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

    //Imf::RgbaInputFile file("../examples/exr_samples/2x2sample.exr");
/*WRITE EXR RGBA TEST*/
    {
        int width = 2;
        int height = 2;
        Imf::Rgba* pixelsW = new Imf::Rgba[width * height];
        for(int i=0;i<width*height;++i) 
        {
            pixelsW[i].r = 1.0;
            pixelsW[i].g = 0.5;
            pixelsW[i].b = 0.25;
            pixelsW[i].a = 1.0;
        }   
        Imf::RgbaOutputFile outFile("../test/sample2x2_write.exr", width, height, Imf::WRITE_RGBA);
        outFile.setFrameBuffer(pixelsW, 1, width);
        outFile.writePixels(height);
        delete[] pixelsW;
    }


    std::vector<half> pixels{};
    Imf::InputFile fileInput ("../examples/exr_samples/sample2x2.exr");
    //Imf::InputFile fileInput ("../test/2x2sample_write.exr");

    std::cout << "compression: " << fileInput.header().compression() << '\n';

    Imath::Box2i dw = fileInput.header().dataWindow();
    int width    = dw.max.x - dw.min.x + 1;
    int height   = dw.max.y - dw.min.y + 1;

    int dx = dw.min.x;
    int dy = dw.min.y;

    pixels.resize(width * height * 4);

    std::cout << "dw.min.x: " << dw.min.x << " dw.min.y: " << dw.min.y << '\n';
    std::cout << "width: " << width << " height: " << height << '\n';

    Imf::FrameBuffer frameBuffer;

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

    for (int i = 0; i < width*height; ++i)
    {
        std::cout << "Pixel<" << i << "> " << pixels[i*4] << ", " <<  pixels[i*4 + 1] << ", " << pixels[i*4 + 2] << ", " << pixels[i*4 + 3] << '\n';
    }

    const Imf::ChannelList &channels = fileInput.header().channels();
    for (Imf::ChannelList::ConstIterator i = channels.begin(); i != channels.end(); ++i)
    {
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
        
    /*
    std::set<std::string> layerNames;
    for (Imf::ChannelList::ConstIterator i = channels.begin(); i != channels.end(); ++i) {
        std::string name = i.name();
        std::cout << i.name() << '\n';

        auto pos = name.find('.');
        if (pos != std::string::npos)
            layerNames.insert(name.substr(0, pos));
    }
*/
    //channels.layers(layerNames);
/*
    for (auto i = layerNames.begin(); i != layerNames.end(); ++i)
        std::cout << "EXR Layer: " << *i << '\n'; 
*/


    
    /*
    RGBA
    Diffuse
    Specular
    */

    /*
    frameBuffer.insert (
        "Z",
        Slice (
            FLOAT,
            (char*) &pixels[-dy][-dx].z,
            sizeof (pixels[0][0]) * 1,
            sizeof (pixels[0][0]) * width));

    file.setFrameBuffer (frameBuffer);
    file.readPixels (dw.min.y, dw.max.y);
*/

    
    return 0;
}
