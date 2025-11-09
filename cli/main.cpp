#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <iostream>

using namespace dmxdenoiser;

int main(int argc, char** argv)
{
    std::cout << "DMX Denoiser run.\nArgs: ";

    for(int i = 0; i < argc; ++i)
        std::cout << argv[i] << ' ';
    
    DMX_LOG_INIT(DMX_MIN_LOG_LEVEL, &std::clog, "./dmxdenoiser.log");

    return 0;
}
