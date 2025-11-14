#include <dmxdenoiser/Options.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/Parser.hpp>
#include <dmxdenoiser/Logger.hpp>

#include <cstddef>
#include <string>

namespace dmxdenoiser
{
    /*
    ./dmxdenoiser 
    --start 1 
    --end 100 
    --gpu 0
    --frames 3
    --beauty default
    --albedo albedo
    --normal tech.####.exr:N 
    --depth tech.####.exr:z
    --aluxary
    --output filtered.####.exr


    struct Options
    {
        int ncores = -1;
        bool gpu = false;
        int startFrame;
        int endFrame;
        std::string inputFile{};
        std::string outputFile{};
        bool albedoDivide = true;
        InputFilesMap inputs{};
        FilterMap filters{};
    };
    */

    Options parseArguments(int argc, char** argv) {
        Options opt{};
        for(std::size_t i = 1; i < argc; ++i) {
            if(i + 1 > argc) {
                DMX_LOG_ERROR("Parser", "parseArguments(): \"", argv[i], "\" option requires an argument.");
                throw std::runtime_error(std::string(argv[i]) + " option requires an argument");
            }

            std::string arg{ argv[i] };
            if(arg == "--gpu") {
                opt.gpu = std::atoi(argv[i+1]);
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if((arg == "--ncores") || (arg == "-j")) {
                opt.ncores = std::atoi(argv[i+1]);
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if((arg == "--start") || (arg == "-s")) {
                opt.startFrame = std::atoi(argv[i+1]);
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if((arg == "--end") || (arg == "-e")) {
                opt.endFrame = std::atoi(argv[i+1]);
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if((arg == "--input") || (arg == "-i")) {
                opt.inputFolder = std::string(argv[i+1]);
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if((arg == "--output") || (arg == "-o")) {
                opt.outputFolder = std::string(argv[i+1]);
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if(arg == "--config") {
                // TODO: parse config and create filters here
                //
                // opt.configFile = std::string(argv[i+1]);
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if(arg == "--beauty") {
                // TODO: implement splitting by ":" delimiter
                // opt.inputs[std::string(argv[i+1])] = std::string(argv[i+1]);
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if(arg == "--albedo") {
                // TODO: implement splitting by ":" delimiter
                // opt.inputs[std::string(argv[i+1])] = std::string(argv[i+1]);
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if(arg == "--normal") {
                // TODO: implement splitting by ":" delimiter
                // opt.inputs[std::string(argv[i+1])] = std::string(argv[i+1]);
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if(arg == "--depth") {
                // TODO: implement splitting by ":" delimiter
                // opt.inputs[std::string(argv[i+1])] = std::string(argv[i+1]);
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            }
        }
            
        return opt;
    }

} // namespace dmxdenoiser
