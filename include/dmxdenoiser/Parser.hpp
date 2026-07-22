// Parser.hpp
#pragma once

#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/Options.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>

#include <string_view>

namespace dmxdenoiser
{

constexpr std::string_view kAovDelimiter = ":";

std::vector<Filter> parseFilterConfig(std::string_view filename);

Options parseArguments(int argc, char* argv[]);

} // namespace dmxdenoiser
