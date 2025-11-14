#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/Options.hpp>

#include <iostream>

using namespace dmxdenoiser;

int main(int argc, char** argv)
{
    std::cout << "DMX Denoiser v0.1.0.\n";

    // Init log
    DMX_LOG_INIT(DMX_MIN_LOG_LEVEL, &std::clog, "./dmxdenoiser.log");

    // parse params
    std::cout << argv[argc-1] << '\n';
    for(int i = 1; i < argc; ++i)
            std::cout << argv[i] << ' ';

    return 0;
}
