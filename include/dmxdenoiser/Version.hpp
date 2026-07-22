// Version.hpp
#pragma once

#include <string>
#include <string_view>

namespace dmxdenoiser {

inline constexpr const char* kProductName = "denoise-machine-x";
inline constexpr const char* kProductShortName = "dmxdenoiser";
inline constexpr const char* kBuildVersion = "0.1.0";
inline constexpr const char* kBuildDate = __DATE__ " " __TIME__;

} // namespace dmxdenoiser

