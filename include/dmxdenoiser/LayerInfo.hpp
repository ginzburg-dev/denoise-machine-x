#ifndef DMXDENOISER_LAYER_INFO_H
#define DMXDENOISER_LAYER_INFO_H

#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/MetaData.hpp>

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
        MetaData metadata{}; ///< Metadata for the layer.
        std::size_t offset; ///< Start of channel in the DMXImage flat buffer.

        LayerInfo(std::string_view name_)
            : name{name_} {}
        LayerInfo(std::string_view name_, std::size_t offset_) 
            : name{name_}, offset{offset_} {}
    };

} // namespace dmxdenoiser

#endif // DMXDENOISER_LAYER_INFO_H
