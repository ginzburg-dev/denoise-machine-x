#ifndef DMXDENOISER_FILTER_H
#define DMXDENOISER_FILTER_H

#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/StringConversions.hpp>

#include <string>
#include <string_view>
#include <unordered_map>

namespace dmxdenoiser
{

    /*Abstract Base*/
    class Filter
    {
    public:
        Filter() = default;
        virtual const char* Name() const = 0;
        virtual void apply(DMXImage& img) const = 0;
        virtual void setParams(const ParamDictionary& params) = 0;
        virtual std::string ToString() const = 0; ///< String representation of the filter name and parameters for logging.
        virtual ~Filter() = default;
    };

} // namespace dmxdenoiser

#endif // DMXDENOISER_FILTER_H
