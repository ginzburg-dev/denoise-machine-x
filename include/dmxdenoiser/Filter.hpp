#ifndef DMXDENOISER_FILTER_H
#define DMXDENOISER_FILTER_H

#include <dmxdenoiser/Backend.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Kernel2D.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/StringConversions.hpp>

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace dmxdenoiser
{

    /*Abstract Base*/
    struct Filter
    {
        float strength = 1.0f; ///< Mixing factor, range [0.0, 1.0]. 0 = original, 1 = fully filtered
        bool filterAlpha = false;
        std::vector<int> frames{};
        std::vector<std::string> layers{};
        
        Backend backend = Backend::CPU;
        BackendResource backendResource{};

        virtual const char* Name() const = 0;

        virtual void setParams(const ParamDictionary& params) = 0;

        void apply(const DMXImage& in, DMXImage& out) const { applyImpl(in, out); }
        void apply(DMXImage& inOut) const { 
            DMXImage tmp{ inOut };
            applyImpl(inOut, tmp);
            inOut = std::move(tmp);
        }

        /*
        static void convolve2D(
            DMXImage& inOut,
            const Kernel2D& kernel,
            std::vector<int> frames = {},
            std::vector<std::string> layers = {},
            bool filterAlpha = false);
        */
        static void convolve2D(
            const DMXImage& input,
            DMXImage& output,
            const Kernel2D& kernel,
            std::vector<int> frames = {},
            std::vector<std::string> layers = {},
            bool filterAlpha = false);

        virtual std::string ToString() const = 0; ///< String representation of the filter name and parameters for logging.
        
        virtual ~Filter();

    protected:
        virtual void applyImpl(const DMXImage& in, DMXImage& out) const = 0;
        virtual void resetParams();
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
