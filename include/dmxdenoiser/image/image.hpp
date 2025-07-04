#ifndef DMXDENOISER_IMAGE_IMAGE_H
#define DMXDENOISER_IMAGE_IMAGE_H

#include <dmxdenoiser/image/Pixel.hpp>
#include <string>
#include <map>
#include <string_view>
#include <vector>
#include <memory>

namespace dmxdenoiser
{

enum ChannelSet
{
    GRAY = 1,
    RGB = 3,
    RGBA = 4,
};

struct Rgba
{
    float r, g, b, a;
};

struct Layer
{
    int numChannels{};
    std::string name{};
};

struct ImageOptions
{
    int width{};
    int height{};
    std::vector<Layer> layers{};
    int numFrames{};
};

class Image
{
public:
    Image(int width, int height, int numChannels, const std::vector<std::string>& layers, int numFrames)
        : m_width{width}, 
        m_height{height}, 
        m_numChannels{numChannels},
        m_numLayers{static_cast<int>(layers.size())},
        m_numFrames{numFrames},
        m_layers{layers},
        m_pixels(width * height * numChannels * layers.size() * numFrames)
    {}

    Image(const ImageOptions& opt)
        : m_width{opt.width}, 
        m_height{opt.height}, 
        m_numChannels{opt.numChannels},
        m_numLayers{static_cast<int>(opt.layers.size())},
        m_numFrames{opt.numFrames},
        m_layers{opt.layers},
        m_pixels(opt.width * opt.height * opt.numChannels * opt.layers.size() * opt.numFrames)
    {}

    int width() const { return m_width; }
    int height() const { return m_height; }
    int channels() const { return m_numChannels; }
    int layers() const { return m_numLayers; }
    int frames() const { return m_numFrames; }
    std::vector<float>* get() { return &m_pixels; }
    float* at(int x, int y) { return m_pixels[x + y*m_width]; }
    ~Image() = default;

protected:
    int m_width{};
    int m_height{};
    int m_numChannels{};
    int m_numLayers{};
    int m_numFrames{};
    std::vector<float> m_pixels;
    std::vector<std::string> m_layers{};
};

}

#endif
