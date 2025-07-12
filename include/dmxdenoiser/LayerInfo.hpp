#ifndef DMXDENOISER_LAYER_INFO_H
#define DMXDENOISER_LAYER_INFO_H

#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/MetaData.hpp>
#include <vector>
#include <string>
#include <string_view>

namespace dmxdenoiser
{

    struct LayerInfo
    {
        std::string name{};
        std::vector<ChannelInfo> channels{};
        MetaData metadata{};
        float* ptr = nullptr;

        LayerInfo(std::string_view name_) 
            : name{name_} {}
        LayerInfo(std::string_view name_, float* ptr_) 
            : name{name_}, ptr{ptr_} {}
    };

} // namespace dmxdenoiser

#endif // DMXDENOISER_LAYER_INFO_H
