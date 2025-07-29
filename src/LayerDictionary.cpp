#include <dmxdenoiser/LayerDictionary.hpp>

#include <algorithm>

namespace dmxdenoiser
{

    LayerDictionary::LayerDictionary(const std::vector<std::string>& layerList)
    {
        for(const auto& name : layerList)
            m_layers[name] = LayerInfo{name};
        setLayerOffsets();
    }

    LayerDictionary::LayerDictionary(const InputFilesMap& inputFilesMap)
    {
        for (const auto& [key, aovs] : inputFilesMap)
            for (const auto& [name, layer] : aovs )
            {
                m_layers[name] = LayerInfo{name};
            }
        setLayerOffsets();
    }

    void LayerDictionary::addLayer(const std::string& name)
    {
        auto it = m_layers.find(name);
        if (it == m_layers.end())
            m_layers[name] = LayerInfo{name};
            setLayerOffsets();
    }

    void LayerDictionary::removeLayer(const std::string& name)
    {
        auto it = m_layers.find(name);
        if (it != m_layers.end())
            m_layers.erase(it);
        setLayerOffsets();
    }

    LayerInfo* LayerDictionary::getLayer(const std::string& name)
    {
        auto it = m_layers.find(name);
        return (it != m_layers.end()) ? &it->second : nullptr;
    }

    const LayerInfo* LayerDictionary::getLayer(const std::string& name) const
    {
        auto it = m_layers.find(name);
        return (it != m_layers.end()) ? &it->second : nullptr;
    }
    
    bool LayerDictionary::hasLayer(const std::string& name) const
    {
        auto it = m_layers.find(name);
        return (it != m_layers.end()) ? true : false;
    }

    void LayerDictionary::clear()
    {
        m_layers.clear();
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
