// ChannelInfo.hpp
#pragma once

#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/PixelType.hpp>

#include <cstddef>
#include <sstream>
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

        std::string ToString(std::size_t indent=0) const
        {
            std::ostringstream oss;

            std::string sIndent = "";
            for (size_t i = 0; i < indent; ++i)
                sIndent += " ";

            oss << sIndent << "ChannelInfo: \n";
            oss << sIndent << "    Name: " << name << '\n';
            oss << sIndent << "    PixelType: " << pixelType;
            return oss.str();
        }
    };



} // namespace dmxdenoiser
