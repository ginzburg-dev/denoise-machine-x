// LayerDictionary.hpp
#pragma once

#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/ChannelInfo.hpp>
#include <dmxdenoiser/LayerInfo.hpp>

#include <cstddef>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace dmxdenoiser
{

    class LayerDictionary
    {
    public:
        LayerDictionary() = default;
        LayerDictionary(const std::vector<std::string>& layerList);
        LayerDictionary(std::vector<std::string_view> layerList);
        LayerDictionary(std::initializer_list<std::string_view> layerList)
            : LayerDictionary(std::vector<std::string_view>{layerList}) {}
        explicit LayerDictionary(const AovDictionary& aovDictionary);
        explicit LayerDictionary(const InputFilesMap& inputFilesMap);
        ~LayerDictionary() = default;

        void addLayer(std::string_view name, std::string_view fileLayerName = "");
        void removeLayer(std::string_view name);

        std::unordered_map<std::string, LayerInfo>& data() { return m_layers; }
        const std::unordered_map<std::string, LayerInfo>& data() const { return m_layers; }
        LayerInfo* getLayer(std::string_view name);
        const LayerInfo* getLayer(std::string_view name) const;
        std::size_t size() const { return m_layers.size(); }

        bool hasLayer(std::string_view name) const;
        void clear() { m_layers.clear(); }

    private:
        std::unordered_map<std::string, LayerInfo> m_layers;
        void setLayerOffsets();
    };

} // namespace dmxdenoiser
