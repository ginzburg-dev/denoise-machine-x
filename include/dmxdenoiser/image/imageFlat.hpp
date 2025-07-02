#ifndef DMXDENOISER_IMAGE_IMAGE_FLAT_H
#define DMXDENOISER_IMAGE_IMAGE_FLAT_H

#include <dmxdenoiser/image/image.hpp>
#include <vector>

namespace dmxdenoiser
{
    class ImageFlat : public IImage
    {
    public:
        ImageFlat() = default;
        ImageFlat(int width, int height);

        int width() const override { return m_width; };
        int height() const override { return m_height; };
        Pixel& get(int x, int y);
        
        ~ImageFlat() override {};

    protected:
        std::vector<Pixel> m_pixels{};
    };
}

#endif

