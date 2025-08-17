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
        // Parameters
        Kernel2D m_kernel;

        // Required: unique filter name
        static constexpr const char* StaticClassName() { return "ConvolutionFilter"; }
        const char* Name() const override { return StaticClassName(); }

        ConvolutionFilter() = default;
        explicit ConvolutionFilter(const ParamDictionary& params) {  setParams(params); }

        ConvolutionFilter(ConvolutionFilter&&) noexcept;
        ConvolutionFilter& operator=(ConvolutionFilter&&) noexcept;

        ConvolutionFilter(const ConvolutionFilter&) = delete;
        ConvolutionFilter& operator=(const ConvolutionFilter&) = delete;

        void setParams(const ParamDictionary& params) override;

        std::string ToString() const override;

    protected:
        void resetParams() override { Filter::resetParams(); m_kernel.clear(); };

    private:
        void applyFilter(const DMXImage& in, DMXImage& out) const override;
    };

} // namespace dmxdenoiser
