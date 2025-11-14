// Parser.hpp
#pragma once

#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/Options.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>

namespace dmxdenoiser
{

    Options parseArguments(int argc, char** argv);

    AovDictionary parseAov(
        const std::string& input,
        const std::string& delimiter = ":");

} // namespace dmxdenoiser
