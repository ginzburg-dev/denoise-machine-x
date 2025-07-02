#ifndef DMXDENOISER_IMAGE_IMAGE_H
#define DMXDENOISER_IMAGE_IMAGE_H

#include <dmxdenoiser/image/pixel.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <memory>

namespace dmxdenoiser
{
    class IImage
    {
    public:
        IImage() = default;
        IImage(int width, int height);

        virtual int width() const = 0;
        virtual int height() const = 0;
        virtual Pixel& get(int x, int y) = 0;
        
        virtual ~IImage() = default;
        
    protected:
        int m_width{};
        int m_height{};
    };
}

#endif
