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
        LayerDictionary(const std::vector<std::string>& layerList);
        explicit LayerDictionary(const InputFilesMap& inputFilesMap);
        ~LayerDictionary() = default;

        void addLayer(const std::string& name);
        void removeLayer(const std::string& name);

        std::vector<LayerInfo>& data() { return m_layers; }
        std::optional<LayerInfo> getLayer(const std::string& name) const;
        std::optional<int> getLayerOffset(const std::string& name) const;
        std::size_t size() const { return m_layers.size(); }
        
        bool hasLayer(const std::string& name) const;
    private:
        std::vector<LayerInfo> m_layers;
        void sort();
    };

} // namespace dmxdenoiser
