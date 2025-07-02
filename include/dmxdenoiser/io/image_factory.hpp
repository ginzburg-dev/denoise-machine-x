#ifndef DMXDENOISER_CORE_IMAGE_FACTORY_H
#define DMXDENOISER_CORE_IMAGE_FACTORY_H

#include <dmxdenoiser/io/image.hpp>
#include <dmxdenoiser/core/params.hpp>
#include <string>
#include <memory>

namespace dmxdenoiser::ImageFactory
{
    std::unique_ptr<IImage> load(const dmxdenoiser::Params& params);
}

#endif
