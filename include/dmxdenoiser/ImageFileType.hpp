// ImageFileType.hpp
#pragma once

#include <iostream>
#include <string>
#include <string_view>

namespace dmxdenoiser
{

    enum class ImageFileType
    {
        Unknown = 0,
        EXR,
        JPG,
        JPEG,
        PNG,
    };

    inline constexpr std::string_view ToString(ImageFileType type)
    {
        switch (type)
        {
        case ImageFileType::EXR:  return "Exr";
        case ImageFileType::JPEG : return "Jpeg";
        case ImageFileType::JPG : return "Jpg";
        case ImageFileType::PNG : return "Png";
        default:                return "Unknown";
        }
    }

    inline std::ostream& operator<<(std::ostream& out, ImageFileType type)
    {
        out << ToString(type);
        return out;
    }

    ImageFileType getImageFileType(const std::string& filename);

} // namespace dmxdenoiser
