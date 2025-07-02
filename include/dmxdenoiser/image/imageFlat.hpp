#ifndef DMXDENOISER_IMAGE_IMAGE_FLAT_H
#define DMXDENOISER_IMAGE_IMAGE_FLAT_H

#include <dmxdenoiser/image/image.hpp>

namespace dmxdenoiser
{
    class ImageFlat : public IImage
    {
    public:
        IImage() = default;
        IImage(int width, int height);

        virtual int width() const;
        virtual int height() const;
        virtual Pixel& get(int x, int y);
        
        ~IImage() override {};
    };
}

#endif

