// LayerInfo.hpp
#pragma once

#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace dmxdenoiser
{

    struct LayerInfo
    {
        std::string name{}; ///< Layer name (e.g. "Diffuse", "Specular").
        std::vector<ChannelInfo> channels{}; ///< List of channels in this layer (e.g. R, G, B).
        ParamDictionary metadata{}; ///< Metadata for the layer.
        std::size_t offset; ///< Start of channel in the DMXImage flat buffer.

        LayerInfo(std::string_view name_)
            : name{name_} {}
        LayerInfo(std::string_view name_, std::size_t offset_)
            : name{name_}, offset{offset_} {}
    };

} // namespace dmxdenoiser
