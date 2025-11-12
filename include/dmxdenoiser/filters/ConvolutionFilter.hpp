// ConvolutionFilter.hpp
#pragma once

#include <dmxdenoiser/Pixel.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/FilterKernels.hpp>

#include <memory>

namespace dmxdenoiser
{

    /// @brief ConvolutionFilter applies a 2D convolution kernel to image pixels.
    /// @note Parameters expected in ParamDictionary:
    ///   - "kernel": Kernel2D (2D kernel)
    ///   - "strength": float (blend factor,  optional)     default: 1.0f
    ///   - "frames": std::vector<int> (frames to filter, optional)     default: empty array <all frames>
    ///   - "layers": std::vector<std::string> (layers to filter, optional)     default: empty array <all allowed layers>
    ///   - "filterAlpha": bool (whether to filter alpha channel, optional)     default: false
    struct ConvolutionFilter : public Filter
    {
        // Filter parameters
        Kernel2D m_kernel;

        // Required: unique filter name
        static constexpr const char* StaticClassName() { return "ConvolutionFilter"; }
        const char* Name() const override { return StaticClassName(); }

        ConvolutionFilter() = default;
        explicit ConvolutionFilter(const ParamDictionary& params) {  setParams(params); }

        ConvolutionFilter(ConvolutionFilter&&) noexcept = default;
        ConvolutionFilter& operator=(ConvolutionFilter&&) noexcept = default;

        ConvolutionFilter(const ConvolutionFilter&) = delete;
        ConvolutionFilter& operator=(const ConvolutionFilter&) = delete;

        void setParams(const ParamDictionary& params) override;

    protected:
        void resetParams() override;

        // Apply implementations of the filter. NVI pattern
        void runFilterCPU(
            const DMXImage& input,
            DMXImage& output,
            const std::vector<int>& layers,
            const std::vector<int>& frames
        ) const override;
        
        void runFilterGPU(
            const DMXImage& input,
            DMXImage& output,
            const std::vector<int>& layers,
            const std::vector<int>& frames
        ) const override;

    };

} // namespace dmxdenoiser
