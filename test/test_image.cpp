/*
#include <iostream>
#include <cstddef>

void* operator new(std::size_t sz){
    std::cout << "Allocating: " << sz << '\n';
    return std::malloc(sz);
}
*/
#include <dmxdenoiser/image/Image.hpp>
#include <cassert>
#include <iostream>

int main()
{
    std::cout << "Test Image OUT" << '\n';
    dmxdenoiser::Image img(2, 2, {"RGBA", "Z"}, 2);

    int x = 1;
    int y = 1;
    std::string channel = "Z";
    int frame = 1;

    auto pixel = img.at(x, y, frame, channel);
    pixel.r = 1.0f;
    pixel.g = .02f;
    pixel.b = .04f;
    pixel.a = 0.9f;
    std::cout << "Pixel<" << pixel.r << ", " << pixel.g << ", " << pixel.b << ", " << pixel.a << ">\n";
    
    assert(pixel.r == 1.0f);
    assert(pixel.g == .02f);
    assert(pixel.g == .02f);

    auto layers = img.layerNames();

    for(const auto& l : layers)
        std::cout << l << ' ';
    std::cout << '\n';

    return 0;
}
