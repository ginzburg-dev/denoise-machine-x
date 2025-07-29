// DMXImage.hpp
#pragma once

#include <dmxdenoiser/LayerDictionary.hpp>
#include <dmxdenoiser/Pixel.hpp>

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <OpenEXR/ImfPixelType.h>

namespace dmxdenoiser
{

    inline constexpr int DEFAULT_NUM_CHANNELS = 4; // RGBA global internal fixed size for all images

    struct DMXImageOptions
    {
        int width{};
        int height{};
        int numChannels = DEFAULT_NUM_CHANNELS;
        LayerDictionary layers{};
        int numFrames{};
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
                int numChannels = DEFAULT_NUM_CHANNELS)
            : m_width{width},
            m_height{height},
            m_layers{layers},
            m_numFrames{numFrames},
            m_numChannels{numChannels},
            m_pixels(width * height * numChannels * layers.size() * numFrames)
        {}
        DMXImage(const DMXImageOptions& opt)
            : m_width{opt.width}, 
            m_height{opt.height},
            m_layers{opt.layers},
            m_numFrames{opt.numFrames},
            m_numChannels{opt.numChannels},
            m_pixels(opt.width * opt.height * opt.numChannels * opt.layers.size() * opt.numFrames)
        {}

        int width() const { return m_width; }
        int height() const { return m_height; }
        int numLayers() const { return m_layers.size(); }
        int numFrames() const { return m_numFrames; }
        int numChannels() const { return m_numChannels; }

        void initialize(
            int width, 
            int height, 
            int numFrames, 
            LayerDictionary layers, 
            int numChannels = DEFAULT_NUM_CHANNELS) 
        { 
            m_width = width;
            m_height = height;
            m_numFrames = numFrames;
            m_numChannels = numChannels;
            m_layers = layers;
            std::size_t size =  static_cast<std::size_t>(width) * 
                                static_cast<std::size_t>(height) *
                                static_cast<std::size_t>(numChannels) *
                                layers.size() *
                                static_cast<std::size_t>(numFrames);
            m_pixels.resize(size);
        };

        LayerDictionary getLayers() const { return m_layers; }
        std::vector<float>& data() { return m_pixels; }
        const std::vector<float>& data() const { return m_pixels; }

        int getLayerIndex(const std::string& layer) const 
        {
            auto layerPtr = m_layers.getLayer(layer);
            if (!layerPtr)
                throw std::runtime_error("Layer not found: " + layer);
            return layerPtr->offset;
        };

        std::size_t getPixelIndex(int x, int y, int frame, int layer) const
        {
            return static_cast<std::size_t>(
                (((frame * this->numLayers() + layer) * m_height + y) * m_width + x) * m_numChannels);
        }

        std::size_t getPixelIndex(int x, int y, int frame, const std::string& layer) const 
        {
            int layerIdx = this->getLayerIndex(layer);
            return getPixelIndex(x, y, frame, layerIdx);
        }

        PixelRGBAView at(int x, int y, int frame, int layer) 
        {
            std::size_t idx = getPixelIndex(x, y, frame, layer);
            return { m_pixels[idx], m_pixels[idx + 1], m_pixels[idx + 2], m_pixels[idx + 3] };
        }

        PixelRGBAView at(int x, int y, int frame, const std::string& layer)
        {
            int layerIdx = this->getLayerIndex(layer);
            return at(x, y, frame, layerIdx);
        }

        bool hasLayer(const std::string& layer) const { return m_layers.getLayer(layer); }

        std::string ToString() const
        {
            // TODO: Implement actual logic!
            return "DMXImage";
        }

        void clear() { std::fill(m_pixels.begin(), m_pixels.end(), 0.0f); }

        ~DMXImage() = default;

    private:
        int m_width{};
        int m_height{};
        int m_numFrames{};
        int m_numChannels{};
        LayerDictionary m_layers;
        std::vector<float> m_pixels;
    };

} // namespace dmxdenoiser
