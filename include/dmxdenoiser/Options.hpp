// Options.hpp
#pragma once

#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/Filter.hpp>

#include <string>
#include <string_view>
#include <map>

namespace dmxdenoiser
{

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
        InputFilesMap inputs{};
    };

} // namespace dmxdenoiser

