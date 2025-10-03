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
        Kernel2D m_kernel;

        // Required: unique filter name
        static constexpr const char* StaticClassName() { return "NLMFilter"; }
        const char* Name() const override { return StaticClassName(); }

        NLMFilter() = default;
        explicit NLMFilter(const ParamDictionary& params) {  setParams(params); }

        NLMFilter(NLMFilter&&) noexcept;
        NLMFilter& operator=(NLMFilter&&) noexcept;

        NLMFilter(const NLMFilter&) = delete;
        NLMFilter& operator=(const NLMFilter&) = delete;

        void setParams(const ParamDictionary& params) override;

        std::string ToString() const override;

    protected:
        void resetParams() override { Filter::resetParams(); m_kernel.clear(); };

    private:
        void applyFilter(const DMXImage& in, DMXImage& out) const override;
        void runFilterCPU(const DMXImage& input, DMXImage& output) const;
        void runFilterGPU(const DMXImage& input, DMXImage& output) const;
        void runFilterMETAL(const DMXImage& input, DMXImage& output) const;
    };

} // namespace dmxdenoiser
