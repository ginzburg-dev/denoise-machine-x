#ifndef DMXDENOISER_CORE_IMAGE_H
#define DMXDENOISER_CORE_IMAGE_H

#include <string>
#include <string_view>

namespace dmxdenoiser
{

    struct Pixel {
        float r, g, b, a;
    };

    class IImage
    {
    public:
        IImage() = default;
        IImage(int width, int height);

        virtual bool load(std::string_view filename) = 0;
        virtual bool save(std::string_view filename) const = 0;

        virtual int width() const = 0;
        virtual int height() const = 0;
        
        virtual Pixel& get(int x, int y) = 0;
        virtual ~IImage() = default;
    };
}

#endif
