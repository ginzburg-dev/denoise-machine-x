#ifndef DMXDENOISER_DENOISER_PARAMS_H
#define DMXDENOISER_DENOISER_PARAMS_H

#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/Filter.hpp>

#include <string>
#include <string_view>
#include <map>

namespace dmxdenoiser
{

    struct DenoiserParams
    {
        int ncores = -1;
        bool gpu = false;
        int startFrame;
        int endFrame;
        std::string inputFileName{};
        std::string outputFileName{};
        bool albedoDivide = true;
        InputFilesMap inputs{};
        FilterMap filters{};
    };

} // namespace dmxdenoiser

#endif // DMXDENOISER_DENOISER_PARAMS_H
