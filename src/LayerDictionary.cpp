#include <dmxdenoiser/LayerDictionary.hpp>

#include <algorithm>

namespace dmxdenoiser
{

    LayerDictionary::LayerDictionary(const std::vector<std::string>& layerList)
    {
        for(const auto& name : layerList)
            m_layers[std::string(name)] = LayerInfo{};
        setLayerOffsets();
    }

    LayerDictionary::LayerDictionary(std::vector<std::string_view> layerList)
    {
        for(const auto& name : layerList)
            m_layers[std::string(name)] = LayerInfo{};
        setLayerOffsets();
    }

    LayerDictionary::LayerDictionary(const AovDictionary& aovDictionary)
    {
        for (const auto& [name, fileLayerName] : aovDictionary )
        {
            m_layers[name] = LayerInfo{fileLayerName};
        }
        setLayerOffsets();
    }

    LayerDictionary::LayerDictionary(const InputFilesMap& inputFilesMap)
    {
        for (const auto& [key, aovs] : inputFilesMap)
            for (const auto& [name, layer] : aovs )
            {
                m_layers[name] = LayerInfo{layer};
            }
        setLayerOffsets();
    }

    void LayerDictionary::addLayer(std::string_view name, std::string_view fileLayerName)
    {
        auto it = m_layers.find(std::string(name));
        if (it == m_layers.end())
        {
            m_layers[std::string(name)] = LayerInfo{fileLayerName};
            setLayerOffsets();
        }
    }

    void LayerDictionary::removeLayer(std::string_view name)
    {
        auto it = m_layers.find(std::string(name));
        if (it != m_layers.end())
            m_layers.erase(it);
        setLayerOffsets();
    }

    LayerInfo* LayerDictionary::getLayer(std::string_view name)
    {
        auto it = m_layers.find(std::string(name));
        return (it != m_layers.end()) ? &it->second : nullptr;
    }

    const LayerInfo* LayerDictionary::getLayer(std::string_view name) const
    {
        auto it = m_layers.find(std::string(name));
        return (it != m_layers.end()) ? &it->second : nullptr;
    }
    
    bool LayerDictionary::hasLayer(std::string_view name) const
    {
        auto it = m_layers.find(std::string(name));
        return (it != m_layers.end()) ? true : false;
    }

    void LayerDictionary::setLayerOffsets()
    {
        int offset = 0;

        for (auto& [name, layerInfo] : m_layers)
            layerInfo.offset = -1;

        for (const auto& name : DEFAULT_AOVS)
        {
            auto it = m_layers.find(name);
            if (it != m_layers.end())
                it->second.offset = offset++;
        }

        for(auto& [name, layerInfo] : m_layers)
        {
            if (layerInfo.offset == -1)
                layerInfo.offset = offset++;
        }
    }

} // namespace dmxdenoiser
