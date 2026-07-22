#include <dmxdenoiser/Options.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/Parser.hpp>
#include <dmxdenoiser/StringConversions.hpp>
#include <dmxdenoiser/Logger.hpp>

#include <cstddef>
#include <string>
#include <utility>

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
                auto beautySplit = splitString(std::string(argv[i+1]), std::string(kAovDelimiter));
                opt.inputs[beautySplit.first]["beauty"] = 
                    beautySplit.second.empty() ? "default" : beautySplit.second;
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if(arg == "--albedo") {
                auto albedoSplit = splitString(std::string(argv[i+1]), std::string(kAovDelimiter));
                opt.inputs[albedoSplit.first]["albedo"] = 
                    albedoSplit.second.empty() ? "default" : albedoSplit.second;
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if(arg == "--normal") {
                auto normalSplit = splitString(std::string(argv[i+1]), std::string(kAovDelimiter));
                opt.inputs[normalSplit.first]["normal"] = 
                    normalSplit.second.empty() ? "default" : normalSplit.second;
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            } else if(arg == "--depth") {
                auto depthSplit = splitString(std::string(argv[i+1]), std::string(kAovDelimiter));
                opt.inputs[depthSplit.first]["depth"] = 
                    depthSplit.second.empty() ? "default" : depthSplit.second;
                DMX_LOG_DEBUG("Parser", "parseArguments(): read argument \"", 
                    argv[i], "\" = ", argv[i+1]);
            }
        }
            
        return opt;
    }

} // namespace dmxdenoiser
