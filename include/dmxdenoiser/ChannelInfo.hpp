// ChannelInfo.hpp
#pragma once

#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/PixelType.hpp>

#include <cstddef>
#include <string>
#include <string_view>

namespace dmxdenoiser
{

    struct ChannelInfo
    {
        std::string name{};
        PixelType pixelType{};
        ParamDictionary metadata{};
        ChannelInfo(std::string_view name_) : name{name_} {}
    };

} // namespace dmxdenoiser
