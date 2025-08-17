// ConvolutionFilter.hpp
#pragma once

#include <dmxdenoiser/Pixel.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/FilterKernels.hpp>

#include <memory>

namespace dmxdenoiser
{

    struct ConvolutionFilterParams : public FilterParams 
    {
        Kernel2D kernel; ///< Custom convolution kernel (1D array representing 2D matrix)
        virtual std::unique_ptr<FilterParams> clone() const override;
    };

    /// @brief ConvolutionFilter applies a 2D convolution kernel to image pixels.
    /// @note Parameters expected in ParamDictionary:
    ///   - "kernel": Kernel2D (2D kernel)
    ///   - "strength": float (blend factor,  optional)     default: 1.0f
    ///   - "frames": std::vector<int> (frames to filter, optional)     default: empty array <all frames>
    ///   - "layers": std::vector<std::string> (layers to filter, optional)     default: empty array <all allowed layers>
    ///   - "filterAlpha": bool (whether to filter alpha channel, optional)     default: false
    struct ConvolutionFilter : public Filter
    {
        // Required: unique filter name
        static constexpr const char* StaticClassName() { return "ConvolutionFilter"; }
        const char* Name() const override { return StaticClassName(); }

        ConvolutionFilter();
        explicit ConvolutionFilter(const ParamDictionary& paramDict) 
            : ConvolutionFilter() { 
            setParams(paramDict); 
        }

        ConvolutionFilter(ConvolutionFilter&&) noexcept;
        ConvolutionFilter& operator=(ConvolutionFilter&&) noexcept;

        ConvolutionFilter(const ConvolutionFilter&) = delete;
        ConvolutionFilter& operator=(const ConvolutionFilter&) = delete;

        void setParams(const ParamDictionary& params) override;
        ConvolutionFilterParams& params() override;
        const ConvolutionFilterParams& params() const override;
        
        std::string ToString() const override;

        ~ConvolutionFilter() override;
    
    private:
        void applyImpl(const DMXImage& in, DMXImage& out) const override;
        void resetParams() override;
    };

} // namespace dmxdenoiser
