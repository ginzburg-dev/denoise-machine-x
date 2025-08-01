#ifndef DMXDENOISER_FILTER_H
#define DMXDENOISER_FILTER_H

#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/StringConversions.hpp>

#include <string>
#include <string_view>
#include <unordered_map>

namespace dmxdenoiser
{

    /*Abstract Base*/
    struct Filter
    {
        float strength = 1.0f; ///< Mixing factor, range [0.0, 1.0]. 0 = original, 1 = fully filtered
        bool filterAlpha = false;
        std::vector<int> frames{0};
        std::vector<std::string> layers{};

        virtual const char* Name() const = 0;
        virtual void apply(DMXImage& img) const = 0;
        virtual void setParams(const ParamDictionary& params) = 0;
        virtual std::string ToString() const = 0; ///< String representation of the filter name and parameters for logging.
        virtual ~Filter() = default;
    };

} // namespace dmxdenoiser

#endif // DMXDENOISER_FILTER_H
