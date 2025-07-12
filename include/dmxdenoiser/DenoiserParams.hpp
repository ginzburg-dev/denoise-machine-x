#ifndef DMXDENOISER_DENOISER_PARAMS_H
#define DMXDENOISER_DENOISER_PARAMS_H

#include <dmxdenoiser/Aov.hpp>
#include <string>
#include <string_view>
#include <map>

namespace dmxdenoiser
{

    struct DenoiserParams
    {
        int ncores = -1;
        bool gpu = false;
        std::string inputFileName{};
        std::string outputFileName{};
        bool albedoDivide = true;
        InputFileDictionary inputs{};
    };

} // namespace dmxdenoiser

#endif // DMXDENOISER_DENOISER_PARAMS_H
