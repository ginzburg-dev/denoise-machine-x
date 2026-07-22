#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/Options.hpp>
#include <dmxdenoiser/Parser.hpp>

#include <iostream>

using namespace dmxdenoiser;

int main(int argc, char* argv[])
{
    std::cout << "DMX Denoiser v0.1.0.\n";

    // Init log
    DMX_LOG_INIT(DMX_MIN_LOG_LEVEL, &std::clog, "./dmxdenoiser.log");

    Options options { parseArguments(argc, argv) };

    // Create filters

    // Load images

    // Run denoiser (filters, images)

    DMX_LOG_SHUTDOWN;

    return 0;
}
