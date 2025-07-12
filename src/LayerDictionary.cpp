#include <dmxdenoiser/image/DMXImageInfoTypes.hpp>
#include <string>
#include <string_view>
#include <optional>
#include <vector>
#include <map>
#include <cstddef>

namespace dmxdenoiser
{

/*class DMXLayerDictionary
{
public:
    DMXLayerDictionary() = default;
    DMXLayerDictionary(const std::vector<std::string>& layerList);
    void addLayer(const std::string& name);
    void addLayer(const std::vector<std::string>& names);
    void removeLayer(const std::string& name);
    std::optional<DMXLayerInfo> getLayer(const std::string& layerName) const 
    { 
        auto it = m_layers.find(layerName);
        if (it != m_layers.end())
            return it->second;
        else
            return std::nullopt;
    }
    int getLayerIndex(const std::string& layerName) const;
    std::size_t size() const { return m_layers.size(); }
    ~DMXLayerDictionary() = default;

private:
    std::map<std::string, DMXLayerInfo> m_layers{};
    void calculateLayerIndices()
    {
        int count = 0;
        for (auto& [name, layerInfo] : m_layers)
            layerInfo.index = count++;
    }

}; 

DMXLayerDictionary::DMXLayerDictionary(const std::vector<std::string>& layerList)
{
    for(int i = 0; i < layerList; ++i)
    {
        m_layers[layerList[]]
    }
}
*/
} // namespace dmxdenoiser
