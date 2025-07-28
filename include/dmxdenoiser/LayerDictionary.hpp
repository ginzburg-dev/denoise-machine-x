// LayerDictionary.hpp
#pragma once

#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/LayerInfo.hpp>

#include <algorithm>
#include <cstddef>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace dmxdenoiser
{

    class LayerDictionary
    {
    public:
        LayerDictionary() = default;
        LayerDictionary(const std::vector<std::string>& layerList)
        {
            for(const auto& name : layerList)
                if (!hasLayer(name))
                    m_layers.push_back(LayerInfo{name});
            sort();
        }

        explicit LayerDictionary(const InputFilesMap& inputFilesMap)
        {
            for (const auto& [key, aovs] : inputFilesMap)
                for (const auto& [name, layer] : aovs )
                {
                    if (!hasLayer(name))
                        m_layers.push_back(LayerInfo{name});
                }
            sort();
        }
        void addLayer(const std::string& name)
        {
            auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const auto& a){ return a.name == name; });
            if (it == m_layers.end())
                m_layers.push_back(LayerInfo{name});
            else
                *it = LayerInfo{name};
            sort();
        }

        void removeLayer(const std::string& name)
        {
            auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const auto& a){ return a.name == name; });
            if (it != m_layers.end())
                m_layers.erase(it);
            sort();
        }

        std::optional<LayerInfo> getLayer(const std::string& name) const
        { 
            auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const auto& a){ return a.name == name; });
            if (it != m_layers.end())
                return *it;
            return std::nullopt;
        }

        std::vector<LayerInfo>& data() { return m_layers; }

        std::optional<int> getLayerOffset(const std::string& name) const {
            auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const auto& a){ return a.name == name; });
            if (it != m_layers.end())
                return it->offset;
                //return std::distance(m_layers.begin(), it);
            return std::nullopt;
        }
        
        bool hasLayer(const std::string& name) const
        {
            auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const auto& a){ return a.name == name; });
            if (it != m_layers.end())
                return true;
            return false;
        }

        std::size_t size() const { return m_layers.size(); }
        ~LayerDictionary() = default;

    private:
        std::vector<LayerInfo> m_layers;

        void sort()
        {
            std::sort(m_layers.begin(), m_layers.end(), 
                [&](const auto& a, const auto& b){
                    auto ia = std::find(DEFAULT_AOVS.begin(), DEFAULT_AOVS.end(), a.name);
                    auto ib = std::find(DEFAULT_AOVS.begin(), DEFAULT_AOVS.end(), b.name);

                    bool aInList = ia != DEFAULT_AOVS.end();
                    bool bInList = ib != DEFAULT_AOVS.end();

                    if (aInList && bInList)
                        return ia < ib;
                    if (aInList)
                        return true;
                    if (bInList)
                        return false;
                        
                    return false;
            });

            // Set sequential offsets based on current sorted order
            for (int i = 0; i < m_layers.size(); ++i)
                m_layers[i].offset = i;
        }
    };

} // namespace dmxdenoiser
