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
    class Image
    {
    public:
        Image(int width, int height)
            : m_width{width}, m_height{height}, m_pixels(width*height)
        {} 

        int width() const { return m_width; }
        int height() const { return m_height; }

        std::vector<Pixel>* get() { return &m_pixels; }

        Pixel& at(int x, int y) { return m_pixels[x + y*m_width]; }

        ~Image() = default;

    private:
        int m_width;
        int m_height;
        std::vector<Pixel> m_pixels;
    };

    class ImageCollection
    {
    public:
        ImageCollection() = default;

        bool addImage(std::string_view channelName, int frame, Image&& img);
        bool addImage(std::string_view channelName, Image&& img);
        Image& getImage(std::string_view channelName, int frame);
        Pixel& at(std::string_view channelName, int frame, int x, int y);

        ~ImageCollection() = default;
    private:
        int m_frames{};
        int m_width{};
        int m_height{};
        std::map<std::string, std::vector<Image>> m_data{};
    };

}

#endif
