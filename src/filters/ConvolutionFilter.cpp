#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/filters/ConvolutionFilter.hpp>

#include <optional>
#include <iostream>
#include <vector>

namespace dmxdenoiser
{

    void ConvolutionFilter::setParams(const ParamDictionary& params)
    {
        resetParams();

        if (auto v = params.getSingleParam<Kernel2D>("kernel")) 
            kernel.set(*v);
        else
            throw std::runtime_error("Missing required parameter 'kernel'");
    };

    void ConvolutionFilter::applyImpl(const DMXImage& in, DMXImage& out) const
    {
        convolve2D(in, out, kernel, frames, layers, filterAlpha);
    };

    std::string ConvolutionFilter::ToString() const
    {
        return "ConvolutionFilter: " + kernel.ToString();
    };

    void ConvolutionFilter::resetParams() {
        Filter::resetParams();
        kernel.clear();
    }

    REGISTER_FILTER(ConvolutionFilter)

} // namespace dmxdenoiser


