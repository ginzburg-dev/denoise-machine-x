// ImageIO.hpp
#pragma once

#include <dmxdenoiser/ChannelBuffer.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/ImageFileType.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/LayerDictionary.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace dmxdenoiser
{
    /// @brief ImageInfo helper structure for ImageIO class
    /// @note Header: ImageIO.hpp
    struct ImageInfo
    {
        int width{};
        int height{};
        ImageFileType type{};
        LayerDictionary layers{};
        ParamDictionary params{};

        std::string ToString() const;
    };

    /*Abstract*/
    class ImageIO
    {
    public:
        ImageIO() = default;

        static std::unique_ptr<ImageIO> create(const std::string& filename);

        virtual void read(
                const std::string& filename,
                DMXImage& img,
                int frame,
                const AovDictionary& layers = {}) = 0;

        virtual void write(
            const std::string& filename,
            const DMXImage& img,
            const std::vector<std::string>& layers) const = 0;

        virtual ImageInfo getImageInfo(const std::string& filename) const = 0;
        
        virtual ~ImageIO() = default;
    };

} // namespace dmxdenoiser
