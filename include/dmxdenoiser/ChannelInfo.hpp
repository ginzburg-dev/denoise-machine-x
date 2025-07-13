#ifndef DMXDENOISER_CHANNEL_INFO_H
#define DMXDENOISER_CHANNEL_INFO_H

#include <dmxdenoiser/MetaData.hpp>
#include <dmxdenoiser/PixelType.hpp>

#include <string>
#include <string_view>

namespace dmxdenoiser
{

    struct ChannelInfo
    {
        std::string name{};
        PixelType pixelType{};
        MetaData metadata{};
        float* ptr = nullptr; // optional

        ChannelInfo(std::string_view name_) 
            : name{name_} {}
        ChannelInfo(std::string_view name_, float* ptr_) 
            : name{name_}, ptr{ptr_} {}
    };

} // namespace dmxdenoiser

#endif // DMXDENOISER_CHANNEL_INFO_H
