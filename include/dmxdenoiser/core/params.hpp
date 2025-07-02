#ifndef DMXDENOISER_CORE_PARAMS_H
#define DMXDENOISER_CORE_PARAMS_H

#include <string>
#include <vector>
#include <map>

namespace dmxdenoiser
{
    struct Params
    {
        std::string inputPath{};
        std::string outputPath{};
        bool multilayer = true;
        bool albedoDivide = true;
        std::string aovAlbedo{};
        std::string aovNormal{};
        std::string aovDepth{};
        std::string aovVariance{};
        std::vector<std::string> otherAovs{}; // Spec, Diffuse, GI
    };
}

#endif
