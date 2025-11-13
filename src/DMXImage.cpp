#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/DMXImageView.hpp>
#include <dmxdenoiser/LayerDictionary.hpp>
#include <dmxdenoiser/Pixel.hpp>

#include <cstddef>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <OpenEXR/ImfPixelType.h>

namespace dmxdenoiser
{

    DMXImage::DMXImage(
            int width, 
            int height, 
            int numFrames, 
            LayerDictionary layers, 
            int numChannels)
        : m_width{width},
        m_height{height},
        m_layers{layers},
        m_numFrames{numFrames},
        m_numChannels{numChannels},
        m_pixels(width * height * numChannels * layers.size() * numFrames),
        m_initialized{true}
    {}

    DMXImage::DMXImage(const DMXImageOptions& opt)
        : m_width{opt.width}, 
        m_height{opt.height},
        m_layers{opt.layers},
        m_numFrames{opt.numFrames},
        m_numChannels{opt.numChannels},
        m_pixels(opt.width * opt.height * opt.numChannels * opt.layers.size() * opt.numFrames),
        m_initialized{true}
    {}

    void DMXImage::initialize(
        int width, 
        int height, 
        int numFrames, 
        LayerDictionary layers, 
        int numChannels) 
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
        m_initialized = true;
    };

    int DMXImage::getLayerIndex(std::string_view layer) const 
    {
        auto layerPtr = m_layers.getLayer(layer);
        if (!layerPtr)
            throw std::runtime_error("Layer not found: " + std::string(layer));
        return layerPtr->offset;
    };

    std::vector<int> DMXImage::getFilteringLayersIndices() const
    {
        std::vector<int> layers{};
        for(const auto& [name, layerInfo] : m_layers.data())
        {
            auto it = std::find(NOT_FILTERED_AOVS.begin(), NOT_FILTERED_AOVS.end(), name);
            if (it == NOT_FILTERED_AOVS.end())
                layers.push_back(layerInfo.offset);
        }

        return layers;
    }

    std::size_t DMXImage::getPixelIndex(int x, int y, int frame, int layer) const
    {
        if (x < 0 || x >= m_width)
            throw std::runtime_error("X coordinate of pixel is out of bounds.");
        if (y < 0 || y >= m_height)
            throw std::runtime_error("Y coordinate of pixel is out of bounds.");
        if (frame < 0 || frame >= m_numFrames)
            throw std::runtime_error("Frame is out of bounds.");
        if (layer < 0 || layer >= static_cast<int>(m_layers.size()))
            throw std::runtime_error("Layer is out of bounds.");
        return static_cast<std::size_t>(
            (((frame * this->numLayers() + layer) * m_height + y) * m_width + x) * m_numChannels);
    }

    std::size_t DMXImage::getPixelIndex(int x, int y, int frame, std::string_view layer) const 
    {
        int layerIdx = this->getLayerIndex(layer);
        return getPixelIndex(x, y, frame, layerIdx);
    }

    PixelRGBAView DMXImage::at(int x, int y, int frame, int layer) 
    {
        std::size_t idx = getPixelIndex(x, y, frame, layer);
        return { m_pixels[idx], m_pixels[idx + 1], m_pixels[idx + 2], m_pixels[idx + 3] };
    }

    PixelRGBAView DMXImage::at(int x, int y, int frame, const std::string& layer)
    {
        int layerIdx = this->getLayerIndex(layer);
        return at(x, y, frame, layerIdx);
    }
    
    PixelRGBA DMXImage::get(int x, int y, int frame, int layer) const 
    {
        std::size_t idx = getPixelIndex(x, y, frame, layer);
        return { m_pixels[idx], m_pixels[idx + 1], m_pixels[idx + 2], m_pixels[idx + 3] };
    }

    PixelRGBA DMXImage::get(int x, int y, int frame, const std::string& layer) const
    {
        int layerIdx = this->getLayerIndex(layer);
        return get(x, y, frame, layerIdx);
    }

    DMXImage::operator DMXImageView()
    {
        return { m_pixels.data(), m_width, m_height, this->numLayers(), this->numFrames(), this->numChannels() };
    }

    std::string DMXImage::ToString(std::size_t indent) const
    {
        std::string sIndent = "";
            for (size_t i = 0; i < indent; ++i)
                sIndent += " ";

        std::ostringstream oss;
        oss << sIndent << "DMXImage: \n";
        oss << sIndent << "\tDimensions: " << m_width << " x " << m_height << "\n";
        oss << sIndent << "\tFrames: " << m_numFrames << "\n";
        oss << sIndent << "\tChannels: " << m_numChannels << "\n";
        oss << sIndent << "\tLayers: ";
        for (const auto& [name, __] : m_layers.data())
            oss << name << ' ';
        return oss.str();
    }

    void DMXImage::clear() { 
        std::fill(m_pixels.begin(), m_pixels.end(), 0.0f);
        m_pixels.clear();
        m_layers.clear();
        m_width = 0;
        m_height = 0;
        m_numFrames = 0;
        m_numChannels = 0;
        m_initialized = false;
    }

} // namespace dmxdenoiser
