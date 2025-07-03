#include <dmxdenoiser/image/Image.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "DMX Denoiser run.\nArgs: ";
    for(int i = 0; i < argc; ++i)
        std::cout << argv[i] << ' ';

    return 0;
}
