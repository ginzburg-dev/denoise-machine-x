#include <dmxdenoiser/ImageFileType.hpp>
#include <dmxdenoiser/util/FileUtils.hpp>

#include <string>
#include <string_view>

namespace dmxdenoiser
{

    ImageFileType getImageFileType(const std::string& filename)
    {
        auto ext = getFileExtension(filename);

        if (ext == "exr")   return ImageFileType::EXR;
        if (ext == "jpg")   return ImageFileType::JPG;
        if (ext == "jpeg")  return ImageFileType::JPEG;
        if (ext == "png")   return ImageFileType::PNG;

        return ImageFileType::Unknown;
    }

} // namespace dmxdenoiser
