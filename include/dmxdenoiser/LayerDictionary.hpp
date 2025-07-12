#ifndef DMXDENOISER_LAYER_DICTIONARY_H
#define DMXDENOISER_LAYER_DICTIONARY_H

#include <dmxdenoiser/LayerInfo.hpp>
#include <dmxdenoiser/ChannelInfo.hpp>
#include <string>
#include <string_view>
#include <optional>
#include <vector>
#include <map>
#include <cstddef>

namespace dmxdenoiser
{

    class LayerDictionary
    {
    public:
        LayerDictionary() = default;
        LayerDictionary(const std::vector<std::string>& layerList);
        void addLayer(const std::string& name);
        void addLayer(const std::vector<std::string>& names);
        void removeLayer(const std::string& name);
        std::optional<LayerInfo> operator[](const std::string& layerName) const 
        { 
            auto it = m_layers.find(layerName);
            if (it != m_layers.end())
                return it->second;
            else
                return std::nullopt;
        }
        int getLayer(const std::string& name) const;
        int getLayerIdx(const std::string& name) const;
        std::size_t size() const { return m_layers.size(); }
        ~LayerDictionary() = default;

    private:
        std::map<std::string, LayerInfo> m_layers{};
        /*
        void calculateLayerIndices()
        {
            int count = 0;
            for (auto& [name, layerInfo] : m_layers)
                layerInfo.index = count++;
        }
        */
    };

} // namespace dmxdenoiser

#endif // DMXDENOISER_LAYER_DICTIONARY_H
