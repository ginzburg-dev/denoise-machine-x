#ifndef DMXDENOISER_LAYER_DICTIONARY_H
#define DMXDENOISER_LAYER_DICTIONARY_H

#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/LayerInfo.hpp>

#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace dmxdenoiser
{

    class LayerDictionary
    {
    public:
        LayerDictionary() = default;
        LayerDictionary(const std::vector<std::string>& layerList);
        explicit LayerDictionary(const InputFilesMap& inputFilesMap);
        void addLayer(const std::string& name);
        void addLayer(const std::vector<std::string>& names);
        void removeLayer(const std::string& name);
        std::optional<LayerInfo> getLayer(const std::string& name) const
        { 
            auto it = m_layers.find(name);
            if (it != m_layers.end())
                return it->second;
            return std::nullopt;
        }
        int getLayerIndex(const std::string& name) const;
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
