#ifndef DMXDENOISER_EXAMPLE_FILTER_H
#define DMXDENOISER_EXAMPLE_FILTER_H

#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Filter.hpp>

namespace dmxdenoiser
{

    class ExampleFilter : public Filter
    {
        int kernel = 3;
        int radius = 4;
        float sigma = 0.7f;
    public:
        // Required: unique filter name
        static constexpr const char* StaticClassName() { return "ExampleFilter"; }
        const char* Name() const override { return StaticClassName(); };

        ExampleFilter() = default;
        ExampleFilter(const ParamDictionary& params) { setParams(params); };
        ~ExampleFilter() override = default;

        void setParams(const ParamDictionary& params) override;
        void apply(DMXImage& img) const override;

        std::string ToString() const override;
    };

} // namespace dmxdenoiser

#endif
