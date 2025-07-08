/*

*/
#include <dmxdenoiser/image/DMXImage.hpp>
#include <cassert>
#include <iostream>
#include <iostream>
#include <cstddef>

void* operator new(std::size_t sz){
    std::cout << "Allocating: " << sz << '\n';
    return std::malloc(sz);
}

int main()
{
    using namespace dmxdenoiser;

    std::cout << "Test Image OUT" << '\n';
    int initW = 2;
    int initH = 2;
    int initFrames = 2;
    std::vector<std::string> initLayers = {"RGBA", "Specular", "Diffuse", "Z"};
    DMXImage img(initW, initH, initLayers, initFrames);

    ////

    assert(img.width() == initW);
    assert(img.height() == initH);
    assert(img.frames() == initFrames);
    assert(img.data().size() == initW*initH*initFrames*initLayers.size()*Settings::numChannels);

    ////

    std::cout << "Test array order.." << '\n';
    int count = 0;
    for(int frame = 0; frame < img.frames(); ++frame)
        for(int layer = 0; layer < img.numLayers(); ++layer)
            for(int y = 0; y < img.height(); ++y)
                for(int x = 0; x < img.width(); ++x)
                {
                    auto pixel = img.at(x, y, frame, layer);
                    std::size_t idx = img.getPixelIndex(x, y, frame, layer);
                    std::cout << "Setting pixel at idx: " << idx << '\n';
                    pixel.r = count++;
                    pixel.g = count++;
                    pixel.b = count++;
                    pixel.a = count++;
                }
    
    auto& flatPixels = img.data();
    count = 0;
    for (const auto& e : flatPixels)
    {
        assert(e == count++);
        std::cout << e << ' ';
    }
    std::cout << '\n';
    
    ////
        
    auto& layers = img.layerNames();

    for(const auto& l : layers)
        std::cout << l << ' ';
    std::cout << '\n';

    ////

    assert(img.getLayerIndex("Specular") == 1);

    ////

    try 
    {
        auto wrongLayerIdx = img.getLayerIndex("wrong layer");
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        assert(std::string(ex.what()) == "Layer not found: wrong layer");
        //return 1;
    }
    
    /////

    DMXImage copy = img;
    assert(copy.data().size() == img.data().size());

    auto& flatCopyPixels = copy.data();
    for(int i = 0; i < img.data().size(); ++i)
    {
        std::cout << flatCopyPixels[i] << ' ';
        assert(flatCopyPixels[i] == flatPixels[i]);
    }

    ////

    assert(copy.width() == img.width());

    /////

    img.clear();
    copy.clear();

    for(int i = 0; i < img.data().size(); ++i)
    {
        assert(flatPixels[i] == 0.0f);
        assert(flatCopyPixels[i] == 0.0f);
    }

    /////

    DMXImageOptions opt{2, 2, {"RGBA", "Specular", "Diffuse", "Z"}, 2};
    DMXImage img2(opt);

    std::cout << "Test image constructor with opt." << '\n';

    count = 0;
    for(const auto& e : img2.layerNames() )
    {
        std::cout << e << ' ';
        assert(e == img.layerNames()[count++]);
    }
    std::cout << '\n';


    ////

    assert(img2.data().size() == initW*initH*initFrames*initLayers.size()*Settings::numChannels);

    //// 

    assert(img.hasLayer("Wrong layer") == false);
    assert(img.hasLayer("RGBA") == true);

    return 0;
}
