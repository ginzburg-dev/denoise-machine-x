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
        // Default parameters
        float m_strength = 1.0f; // Mixing factor with the original, range [0.0, 1.0]
        bool m_filterAlpha = false;
        Backend m_backend = Backend::CPU;
        BackendResource m_backendResource;
        std::string m_filterInfo{};

        virtual const char* Name() const = 0;

        virtual void setParams(const ParamDictionary& params) = 0;

        void apply(
            const DMXImage& in,
            DMXImage& out, 
            const std::vector<std::string>& layers = {},
            const std::vector<int>& frames = {}
        ) const;
        void apply(
            DMXImage& inOut,
            const std::vector<std::string>& layers = {},
            const std::vector<int>& frames = {}
        ) const;

        // String representation of the filter name and parameters for logging.
        std::string ToString() const; 
        
        virtual ~Filter() = default;

    protected:
        virtual void resetParams() = 0;

        // Apply implementations of the filter. NVI pattern
        virtual void runFilterCPU(
            const DMXImage& input,
            DMXImage& output,
            const std::vector<int>& layers,
            const std::vector<int>& frames
        ) const = 0;

        virtual void runFilterGPU(
            const DMXImage& input,
            DMXImage& output,
            const std::vector<int>& layers,
            const std::vector<int>& frames
        ) const;
        
        virtual void runFilterMETAL(
            const DMXImage& input,
            DMXImage& output,
            const std::vector<int>& layers,
            const std::vector<int>& frames
        ) const;

    private:
        std::vector<int> resolveFrameIndices(
            const DMXImage& input,
            const std::vector<int>& frames
        ) const;
        std::vector<int> resolveLayerIndices(
            const DMXImage& input,
            const std::vector<std::string>& layers
        ) const;

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
