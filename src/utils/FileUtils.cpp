#include <dmxdenoiser/utils/FileUtils.hpp>
#include <dmxdenoiser/StringConversions.hpp>

#include <algorithm>
#include <string>
#include <string_view>
#include <sstream>
#include <cmath>

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

    std::string formatSequencePath(std::string_view filename, int frame, char pattern) {
        int padding = getPadding(filename, pattern);
        if (padding == 0) return std::string{filename};

        std::string frameStr = intToStringPadded(frame, padding);

        auto pos = filename.rfind(pattern);

        int startPos = pos - padding + 1;
        std::string result = std::string{filename};

        result.replace(startPos, padding, frameStr);

        return result;
    }

} // namespace dmxdenoiser
