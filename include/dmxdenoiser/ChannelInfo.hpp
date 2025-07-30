// ChannelInfo.hpp
#pragma once

#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/PixelType.hpp>

#include <cstddef>
#include <string>
#include <string_view>

namespace dmxdenoiser
{
    const std::string DEFAULT_CHANNEL_ORDER = "rgbaz";

    struct ChannelInfo
    {
        std::string name{};
        PixelType pixelType{};
        ParamDictionary metadata{};

        ChannelInfo() = default;
        ChannelInfo(std::string_view name_) : name{name_} {}
        ChannelInfo(std::string_view name_, PixelType pixelType_) 
            : name{name_}, pixelType{pixelType_}
        {}
    };



} // namespace dmxdenoiser
