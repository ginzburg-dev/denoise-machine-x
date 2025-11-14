// Options.hpp
#pragma once

#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>

#include <string>
#include <string_view>
#include <map>

namespace dmxdenoiser
{
    struct FilterConfig 
    {
        std::string name{};
        ParamDictionary params{};
    };
    
    struct Options
    {
        int ncores = -1;
        bool gpu = false;
        int startFrame;
        int endFrame;
        std::string inputFolder{};
        std::string outputFolder{};
        std::string configFile{};
        bool albedoDivide = true;
        InputFileAovMap inputs{};
        std::vector<FilterConfig> filters{};
    };

} // namespace dmxdenoiser

