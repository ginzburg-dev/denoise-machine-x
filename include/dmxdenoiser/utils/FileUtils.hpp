// FileUtils.hpp
#pragma once

#include <string>
#include <string_view>

namespace dmxdenoiser
{

    std::string getFileExtension(std::string_view filename);

    std::string getFileBaseName(std::string_view filename);

    std::string formatSequencePath(std::string_view filename, int frame, char pattern = '#');

} // namespace dmxdenoiser
