// ParamDictionary.hpp

#pragma once

#include <map>
#include <string>
#include <vector>

namespace dmxdenoiser
{

    struct ParamDictionary
    {
        std::map<std::string, std::vector<int>> intParams;
        std::map<std::string, std::vector<float>> floatParams;
        std::map<std::string, std::vector<std::string>> stringParams;
        std::map<std::string, std::vector<bool>> boolParams;

        
    };

} // namespace dmxdenoiser

