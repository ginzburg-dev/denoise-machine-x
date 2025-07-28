#ifndef DMXDENOISER_AOV_H
#define DMXDENOISER_AOV_H

#include <algorithm>
#include <array>
#include <string>
#include <string_view>
#include <map>
#include <variant>

namespace dmxdenoiser
{

    /// @brief Default AOVs required by the denoiser.
    /// @details
    /// These AOVs are mandatory for denoising operations.
    /// They typically include essential render passes like beauty, albedo, normal, depth, and position.
    const std::array<std::string, 5> DEFAULT_AOVS = {
        "beauty", "albedo", "normal", "depth", "position" 
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
    * @brief Dictionary mapping input file paths to their AOV dictionaries.
    * Maps: file path (e.g., "/path/to/image.exr") -> AOV dictionary for that file.
    * The AOV dictionary maps AOV names (e.g., "albedo", "depth") to layer names inside the file.
    * Example:
    *   InputFileDictionary files = {
    *      {"/images/shot01.exr", {{"albedo", "Diffuse"}, {"depth", "Z"}}},
    *      {"/images/shot02.exr", {{"beauty", "default"}}}
    *   };
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

} // namespace dmxdenoiser

#endif // DMXDENOISER_AOV_H
