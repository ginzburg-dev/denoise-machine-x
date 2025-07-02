#ifndef DMXDENOISER_IO_IMAGEIO_FACTORY_H
#define DMXDENOISER_IO_IMAGEIO_FACTORY_H

#include <dmxdenoiser/image/image.hpp>
#include <dmxdenoiser/core/params.hpp>
#include <dmxdenoiser/io/params/imageIOParams.h>
#include <string>
#include <vector>
#include <memory>

namespace dmxdenoiser::io
{
    class imageIOFactory
    {
    public:
        static std::unique_ptr<IImage> ImageLoad(const dmxdenoiser::io::IImageIOParams& params);

    private:
        
    };
    
}

#endif
