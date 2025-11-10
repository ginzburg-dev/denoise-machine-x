#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <iostream>

using namespace dmxdenoiser;

int main(int argc, char** argv)
{
    std::cout << "DMX Denoiser v0.1.0.\n";

    // Init log
    DMX_LOG_INIT(DMX_MIN_LOG_LEVEL, &std::clog, "./dmxdenoiser.log");

    // parse params
    for(int i = 0; i < argc; ++i)
            std::cout << argv[i] << ' ';

    return 0;
}
