// BoxFilter.hpp
#pragma once

#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Filter.hpp>

namespace dmxdenoiser
{
    /*
    /// BoxFilter parameters:
    /// int kernel
    struct BoxFilter : public Filter
    {
        // Parameters
        int kernel = 1;
        
        // Required: unique filter name
        static constexpr const char* StaticClassName() { return "BoxFilter"; }
        const char* Name() const override { return StaticClassName(); };

        BoxFilter() = default;
        BoxFilter(const ParamDictionary& params) { setParams(params); };
        ~BoxFilter() override = default;

        void setParams(const ParamDictionary& params) override;
        void apply(DMXImage& img) const override;

        std::string ToString() const override;
    };

    void applySimpleBoxFilter(DMXImage& img);
    */
} // namespace dmxdenoiser
