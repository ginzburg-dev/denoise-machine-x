#ifndef DMXDENOISER_IO_EXR_IO_PARAMS_H
#define DMXDENOISER_IO_EXR_IO_PARAMS_H

#include <dmxdenoiser/io/params/imageIOParams.h>

namespace dmxdenoiser::io
{
    struct EXRIOParams : public IImageIOParams
    {
        bool multilayer = false;
        ~EXRIOParams() override {};
    };
}

#endif
