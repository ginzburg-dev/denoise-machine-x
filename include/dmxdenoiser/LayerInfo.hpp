// LayerInfo.hpp
#pragma once

#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>

#include <algorithm>
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
        int offset = -1; ///< Start of channel in the DMXImage flat buffer.

        LayerInfo() = default;
        LayerInfo(std::string_view name_)
            : name{name_} {}
        LayerInfo(std::string_view name_, int offset_)
            : name{name_}, offset{offset_} {}

        void addChannel(std::string_view name, PixelType pixelType = PixelType::Unknown)
        {
            ChannelInfo channel{name, pixelType};
            auto it = std::find_if(channels.begin(), channels.end(), [&](const auto& s){ return s.name == name; });
            if (it != channels.end())
                *it = channel;
            else
                channels.push_back(channel);

            sortChannels();
        }

        ChannelInfo* getChannel(std::string_view name)
        {
            auto it = std::find_if(channels.begin(), channels.end(), [&](const auto& s){ return s.name == name; });
            return it != channels.end() ? &(*it) : nullptr;
        }

        const ChannelInfo* getChannel(std::string_view name) const
        {
            auto it = std::find_if(channels.begin(), channels.end(), [&](const auto& s){ return s.name == name; });
            return it != channels.end() ? &(*it) : nullptr;
        }

        void sortChannels(const std::string& order = DEFAULT_CHANNEL_ORDER)
        {
            std::sort(channels.begin(), channels.end(),
                [&](const auto& a, const auto& b){
                    auto ia = order.find(a.name.empty() ? '\0' : std::tolower(a.name[0]));
                    auto ib = order.find(b.name.empty() ? '\0' : std::tolower(b.name[0]));

                    bool aInList = (ia != std::string::npos);
                    bool bInList = (ib != std::string::npos);

                    if (aInList && bInList)
                        return ia < ib;
                    if (aInList)
                        return true;
                    if (bInList)
                        return false;
                    return false;
            });
        }
    };

} // namespace dmxdenoiser
