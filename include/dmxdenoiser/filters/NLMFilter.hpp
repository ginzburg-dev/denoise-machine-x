// NLMFilter.hpp
#pragma once

#include <dmxdenoiser/Pixel.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/FilterKernels.hpp>

#include <memory>

namespace dmxdenoiser
{

    struct NLMFilter : public Filter
    {
        // Parameters
        int m_radius = 4;
        int m_patchRadius = 3;
        float m_sigmaBeauty = 1.f;
        float m_sigmaAlbedo = 1.f;
        float m_sigmaNormal = 1.f;
        float m_sigmaDepth = 1.f;

        // Required: unique filter name
        static constexpr const char* StaticClassName() { return "NLMFilter"; }
        const char* Name() const override { return StaticClassName(); }

        NLMFilter() = default;
        explicit NLMFilter(const ParamDictionary& params) {  setParams(params); }

        NLMFilter(NLMFilter&&) noexcept = default;
        NLMFilter& operator=(NLMFilter&&) noexcept = default;

        NLMFilter(const NLMFilter&) = delete;
        NLMFilter& operator=(const NLMFilter&) = delete;

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

    };

} // namespace dmxdenoiser
