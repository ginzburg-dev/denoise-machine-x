// ConvolutionFilter.hpp
#pragma once

#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/FilterKernels.hpp>

namespace dmxdenoiser
{
    /// @brief ConvolutionFilter that applies a custom kernel to an image.
    struct ConvolutionFilter : public Filter
    {
        // Parameters
        float strength = 1.0f; ///< Mixing factor, range [0.0, 1.0]. 0 = original, 1 = fully filtered
        Kernel2D kernel; ///< Custom convolution kernel (1D array representing 2D matrix)
        
        // Required: unique filter name
        static constexpr const char* StaticClassName() { return "ConvolutionFilter"; }
        const char* Name() const override { return StaticClassName(); };

        ConvolutionFilter() = default;
        ConvolutionFilter(const ParamDictionary& params) { setParams(params); };
        ~ConvolutionFilter() override = default;

        void setParams(const ParamDictionary& params) override;
        void apply(DMXImage& img) const override;

        std::string ToString() const override;
    };

} // namespace dmxdenoiser
