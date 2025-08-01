// DMXImage.hpp
#pragma once

#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/LayerDictionary.hpp>
#include <dmxdenoiser/Pixel.hpp>

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace dmxdenoiser
{

    inline constexpr int DEFAULT_NUM_CHANNELS = 4; // RGBA global internal fixed size for all images

    struct DMXImageOptions
    {
        int width{};
        int height{};
        int numFrames{};
        LayerDictionary layers{};
        int numChannels = DEFAULT_NUM_CHANNELS;
    };

    class DMXImage
    {
    public:
        DMXImage() = default;
        DMXImage(
                int width, 
                int height, 
                int numFrames, 
                LayerDictionary layers, 
                int numChannels = DEFAULT_NUM_CHANNELS);
        DMXImage(const DMXImageOptions& opt);

        void initialize(
            int width, 
            int height, 
            int numFrames, 
            LayerDictionary layers, 
            int numChannels = DEFAULT_NUM_CHANNELS);

        int width() const { return m_width; }
        int height() const { return m_height; }
        int numLayers() const { return m_layers.size(); }
        int numFrames() const { return m_numFrames; }
        int numChannels() const { return m_numChannels; }

        LayerDictionary& getLayers() { return m_layers; }
        const LayerDictionary& getLayers() const { return m_layers; }

        std::vector<float>& data() { return m_pixels; }
        const std::vector<float>& data() const { return m_pixels; }

        int getLayerIndex(std::string_view layer) const;
        std::vector<int> getFilteringLayersIndices() const;

        std::size_t getPixelIndex(int x, int y, int frame, int layer) const;
        std::size_t getPixelIndex(int x, int y, int frame, std::string_view layer) const;

        PixelRGBAView at(int x, int y, int frame, int layer);
        PixelRGBAView at(int x, int y, int frame, const std::string& layer);

        bool isInitialized() const { return m_initialized; }
        bool hasLayer(const std::string& layer) const { return m_layers.getLayer(layer); }
        bool hasValidLayerDictionary() const { return m_layers.size() != 0; }
        
        std::string ToString() const;

        void clear();

        ~DMXImage() = default;

    private:
        bool m_initialized{ false };

        int m_width{};
        int m_height{};
        int m_numFrames{};
        int m_numChannels{};
        LayerDictionary m_layers;
        std::vector<float> m_pixels;

    };

} // namespace dmxdenoiser
