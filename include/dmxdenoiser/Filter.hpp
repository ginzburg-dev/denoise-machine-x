#ifndef DMXDENOISER_FILTER_H
#define DMXDENOISER_FILTER_H

#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/StringConversions.hpp>

#include <map>
#include <string>
#include <string_view>

namespace dmxdenoiser
{

    /*Abstract Base*/
    struct Filter
    {
        float strength = 1.0f; ///< Mixing factor, range [0.0, 1.0]. 0 = original, 1 = fully filtered
        bool filterAlpha = false;
        std::vector<int> frames{};
        std::vector<std::string> layers{};

        virtual const char* Name() const = 0;
        virtual void apply(DMXImage& img) const = 0;
        virtual void setParams(const ParamDictionary& params) = 0;
        virtual std::string ToString() const = 0; ///< String representation of the filter name and parameters for logging.
        virtual ~Filter() = default;

    protected:
        virtual void resetParams(){
            strength = 1.0f;
            filterAlpha = false;
            frames.clear();
            layers.clear();
        };
    };

    /**
    * @brief Maps filter names to their parameter dictionaries.
    *
    * Associates a filter's unique string name (e.g., "BoxFilter", "BilateralFilter") 
    * with a ParamDictionary holding all parameters for that filter instance.
    *
    * Example:
    *   FilterMap filters = {
    *      {"BoxFilter", {{"kernel", 3}, {"strength", 0.5f}}},
    *      {"SobelFilter", {{"direction", "horizontal"}}}
    *   };
    *
    * @note Header: Filter.hpp
    */
    using FilterMap = std::map<std::string, ParamDictionary>;

} // namespace dmxdenoiser

#endif // DMXDENOISER_FILTER_H
