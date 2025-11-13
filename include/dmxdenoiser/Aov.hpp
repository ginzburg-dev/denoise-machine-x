// Aov.hpp
#pragma once

#include <algorithm>
#include <string>
#include <sstream>
#include <string_view>
#include <map>
#include <vector>

namespace dmxdenoiser
{

    /// @brief Default AOVs required by the denoiser.
    /// @details
    /// These AOVs are mandatory for denoising operations.
    /// They typically include essential render passes like beauty, albedo, normal, depth, and position.
    const std::vector<std::string> DEFAULT_AOVS = {
        "beauty", "albedo", "normal", "depth", "position" 
    };

    const std::vector<std::string> NOT_FILTERED_AOVS = {
        "normal", "depth", "position"
    };

    /**
     * @brief Dictionary of AOVs mapped to layer names.
     * Maps an AOV name (e.g., "albedo", "depth", "beauty") to the corresponding layer name inside the file
     * (e.g., "Diffuse", "Depth", "default").
     * Example:
     *   AovDictionary dict = {
     *      {"albedo", "Diffuse"},
     *      {"depth", "Z"},
     *      {"beauty", "default"}
     *   };
     */
    using AovDictionary = std::map<std::string, std::string>;

    /**
    * @brief Mapping input file paths to their AOV dictionaries.
    * Maps: file path (e.g., "/path/to/image.exr") -> AOV dictionary for that file.
    * The AOV dictionary maps AOV names (e.g., "albedo", "depth") to layer names inside the file.
    * Example:
    *   InputFilesMap files = {
    *      {"/images/shot01.exr", {{"albedo", "Diffuse"}, {"depth", "Z"}}},
    *      {"/images/shot02.exr", {{"beauty", "default"}}}
    *   };
    * @note Header: Aov.hpp
    */
    using InputFilesMap = std::map<std::string, AovDictionary>;

    inline bool hasRequiredAovs(InputFilesMap& dict)
    {   
        for (const auto& [filename, aovs] : dict)
        {
            for (const auto& [name, layer] : aovs)
            {
                auto it = std::find(DEFAULT_AOVS.begin(), DEFAULT_AOVS.end(), name);
                if (it == DEFAULT_AOVS.end() || layer.empty() )
                    return false;
            }
        }
        return true;
    }

    inline std::string ToString(const AovDictionary& aovs)
    {   
        std::ostringstream oss;
        oss << "AovDictionary:\n";
        for(const auto& [l, r] : aovs)
            oss << "\t[" << l << ", " << r << "]\n";
        return oss.str();
    }

} // namespace dmxdenoiser
