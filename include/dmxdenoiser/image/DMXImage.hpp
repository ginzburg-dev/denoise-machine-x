#ifndef DMXDENOISER_IMAGE_IMAGE_H
#define DMXDENOISER_IMAGE_IMAGE_H

/*
ptr math in DMX image map
*/
#include <OpenEXR/ImfPixelType.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <string_view>
#include <cstddef> // for std::size_t
#include <optional>

namespace dmxdenoiser
{

enum ChannelSet
{
    kGRAY = 1,
    kRGB = 3,
    kRGBA = 4,
};

namespace Settings
{
    inline constexpr int numChannels = kRGBA;
}

struct Pixel
{
    float& r;
    float& g;
    float& b;
    float& a;
};

struct DMXImageOptions
{
    int width{};
    int height{};
    std::vector<std::string> layers{};
    int numFrames{};
};

struct DMXChannel
{
    std::string name{};
    std::optional<Imf::PixelType> pixelType{};
    std::string metadata{};
    void* ptr = nullptr;

    DMXChannel() = default;
    DMXChannel(std::string_view name_)
        : name{name_}
    {}
    DMXChannel(std::string_view name_, Imf::PixelType pixelType_)
        : name{name_}, pixelType{pixelType_}
    {}
    DMXChannel(std::string_view name_, void* ptr_)
        : name{name_}, ptr{ptr_}
    {}
};

struct DMXLayer
{
    std::string name{};
    std::vector<DMXChannel> channels{};
    std::string metadata{};
    void* ptr = nullptr;

    DMXLayer() = default;
    DMXLayer(std::string_view name_)
        : name{name_}
    {}
    DMXLayer(std::string_view name_, void* ptr_)
        : name{name_}, ptr{ptr_}
    {}
    DMXLayer(std::string_view name_, const std::vector<DMXChannel>& channels_)
        : name{name_}, channels{channels_}
    {}
};

// Hash functor to enable efficient heterogeneous lookup in unordered_map (C++20)
struct TransparentHash : std::hash<std::string_view> {
    using is_transparent = void;
};

class DMXImage
{
public:
    DMXImage(int width, int height, const std::vector<std::string>& layers, int numFrames)
        : m_width{width},
        m_height{height},
        m_layers{layers},
        m_numFrames{numFrames},
        m_pixels(width * height * Settings::numChannels * layers.size() * numFrames)
    {
        buildLayerToIndex(layers);
    }

    DMXImage(const DMXImageOptions& opt)
        : m_width{opt.width}, 
        m_height{opt.height},
        m_layers{opt.layers},
        m_numFrames{opt.numFrames},
        m_pixels(opt.width * opt.height * Settings::numChannels * opt.layers.size() * opt.numFrames)
    {
        buildLayerToIndex(opt.layers);
    }

    int width() const { return m_width; }
    int height() const { return m_height; }
    int numLayers() const { return m_layers.size(); }
    const std::vector<std::string>& layerNames() const { return m_layers; }
    bool hasLayer(std::string_view layer) const { return m_layerNameToIndex.contains(layer); }
    int getLayerIndex(std::string_view layer) const 
    {
        auto it = m_layerNameToIndex.find(layer);
        if (it == m_layerNameToIndex.end())
            throw std::runtime_error("Layer not found: " + std::string(layer));
        return it->second; 
    };
    int frames() const { return m_numFrames; }
    //float* data() { return m_pixels.data(); }
    std::vector<float>& data() { return m_pixels; }
    const std::vector<float>& data() const { return m_pixels; }
    std::size_t getPixelIndex(int x, int y, int frame, int layer) const
    {
        return static_cast<std::size_t>(
            (((frame*this->numLayers() + layer)*m_height + y)*m_width + x)*Settings::numChannels);
    }

    std::size_t getPixelIndex(int x, int y, int frame, std::string_view layer) const 
    {
        int layerIdx = this->getLayerIndex(layer);
        return getPixelIndex(x, y, frame, layerIdx);
    }

    Pixel at(int x, int y, int frame, int layer) 
    {
        std::size_t idx = getPixelIndex(x, y, frame, layer);
        return { m_pixels[idx], m_pixels[idx + 1], m_pixels[idx + 2], m_pixels[idx + 3] };
    }

    Pixel at(int x, int y, int frame, std::string_view layer)
    {
        int layerIdx = this->getLayerIndex(layer);
        return at(x, y, frame, layerIdx);
    }

    void clear() { 
        /*memset(m_pixels.data(), 0, sizeof(float)*m_pixels.size());*/
        std::fill(m_pixels.begin(), m_pixels.end(), 0.0f); 
    }

    ~DMXImage() = default;

private:
    int m_width{};
    int m_height{};
    int m_numFrames{};
    std::vector<std::string> m_layers;
    std::vector<float> m_pixels;
    std::unordered_map<std::string, int, TransparentHash, std::equal_to<>> m_layerNameToIndex;

    void buildLayerToIndex(const std::vector<std::string>& layers)
    {
        for (int i = 0; i < layers.size(); ++i)
            m_layerNameToIndex[layers[i]] = i;
    }
};

}

#endif
