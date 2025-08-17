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
        float m_strength = 1.0f; // Mixing factor with the original, range [0.0, 1.0]
        bool m_filterAlpha = false;
        std::vector<int> m_frames;
        std::vector<std::string> m_layers;
        Backend m_backend = Backend::CPU;
        BackendResource m_backendResource{};

        virtual const char* Name() const = 0;
        virtual void setParams(const ParamDictionary& params) = 0;
        void apply(const DMXImage& in, DMXImage& out) const { applyFilter(in, out); }
        void apply(DMXImage& inOut) const { 
            DMXImage tmp{ inOut };
            applyFilter(inOut, tmp);
            inOut = std::move(tmp);
        }

        // String representation of the filter name and parameters for logging.
        virtual std::string ToString() const = 0; 
        
        virtual ~Filter() = default;

    protected:
        virtual void resetParams() = 0;

    private:
        // Apply implementation of the filter
        virtual void applyFilter(const DMXImage& in, DMXImage& out) const = 0; 
    };

    inline void Filter::resetParams() {
        m_strength = 1.0f; m_filterAlpha = false;
        m_frames.clear(); m_layers.clear();
        m_backend = Backend::CPU; m_backendResource = {};
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
