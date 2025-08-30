#include <dmxdenoiser/utils/FileUtils.hpp>

#include <algorithm>
#include <string>
#include <string_view>

namespace dmxdenoiser
{
    
    std::string getFileExtension(std::string_view filename)
    {
        auto pos = filename.find_last_of('.');
        if (pos == std::string_view::npos)
            return {};
        std::string ext = std::string(filename.substr(pos + 1));
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });
        return ext;
    }

    std::string getFileBaseName(std::string_view filename) {
        auto pos = filename.find_last_of("/\\");
        if (pos == std::string_view::npos)
            return {};
        return std::string(filename.substr(pos+1));
    }

} // namespace dmxdenoiser
