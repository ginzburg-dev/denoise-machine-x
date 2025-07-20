// ParamDictionary.hpp

#pragma once

#include <dmxdenoiser/util/AlwaysFalse.hpp>

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace dmxdenoiser
{

    struct ParamDictionary
    {
        std::map<std::string, std::vector<int>> ints;
        std::map<std::string, std::vector<float>> floats;
        std::map<std::string, std::vector<bool>> bools;
        std::map<std::string, std::vector<std::string>> strings;

        // Set a single-value parameter
        void addInt(const std::string& name, int value) { ints[name] = { value }; }
        void addFloat(const std::string& name, float value) { floats[name] = { value }; }
        void addBool(const std::string& name, bool value) { bools[name] = { value }; }
        void addString(const std::string& name, const std::string& value) { strings[name] = { value }; }
        
        // Set a vector-value parameter
        void addIntArray(const std::string& name, std::vector<int> value) { ints[name] = value; }
        void addFloatArray(const std::string& name, std::vector<float> value) { floats[name] = value; }
        void addBoolArray(const std::string& name, std::vector<bool> value) { bools[name] = value; }
        void addStringArray(const std::string& name, std::vector<std::string> value) { strings[name] = value; }

        // Get a single-value parameter
        template<typename T>
        std::optional<T> getSingleParam(const std::string& name) const {
            if constexpr (std::is_same_v<T, int>){
                auto it = ints.find(name);
                if (it != ints.end() && !it->second.empty())
                    return it->second[0];
            } else if constexpr (std::is_same_v<T, float>) {
                auto it = floats.find(name);
                if (it != floats.end() && !it->second.empty())
                    return it->second[0];
            } else if constexpr (std::is_same_v<T, bool>) {
                auto it = bools.find(name);
                if (it != bools.end() && !it->second.empty())
                    return it->second[0];
            } else if constexpr (std::is_same_v<T, std::string>) {
                auto it = strings.find(name);
                if (it != strings.end() && !it->second.empty())
                    return it->second[0];
            } else {
                static_assert(always_false<T>::value, "Type not supported!");
            }
            return std::nullopt;
        }

        // Get a vector-value parameter
        template<typename T>
        std::optional<std::vector<T>> getArrayParam(const std::string& name) const {
            if constexpr (std::is_same_v<T, int>){
                auto it = ints.find(name);
                if (it != ints.end())
                    return it->second;
            } else if constexpr (std::is_same_v<T, float>) {
                auto it = floats.find(name);
                if (it != floats.end())
                    return it->second;
            } else if constexpr (std::is_same_v<T, bool>) {
                auto it = bools.find(name);
                if (it != bools.end())
                    return it->second;
            } else if constexpr (std::is_same_v<T, std::string>) {
                auto it = strings.find(name);
                if (it != strings.end())
                    return it->second;
            } else {
                static_assert(!std::is_same_v<T, T>, "Type not supported!");
            }
            return std::nullopt;
        }
    };

} // namespace dmxdenoiser

