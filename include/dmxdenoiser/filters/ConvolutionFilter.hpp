// ConvolutionFilter.hpp
#pragma once

#include <dmxdenoiser/Pixel.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/FilterKernels.hpp>

namespace dmxdenoiser
{
    /// @brief ConvolutionFilter that applies a custom kernel to an image.
    struct ConvolutionFilter : public Filter
    {
        // Parameters
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

    private:
        void convolveSimple(DMXImage& img) const;

    };
    

} // namespace dmxdenoiser
