#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/filters/ConvolutionFilter.hpp>
#include <dmxdenoiser/Logger.hpp>

#include <optional>
#include <iostream>
#include <vector>

namespace dmxdenoiser
{

    void ConvolutionFilter::setParams(const ParamDictionary& params)
    {
        resetParams();

        if (auto v = params.getSingleParam<Kernel2D>("kernel"))
        {
            kernel.set(*v);
            DMX_LOG_INFO("ConvolutionFilter", "Kernel parameter set sucessfully.");
        }
        else
        {
            DMX_LOG_ERROR("ConvolutionFilter", "Missing required parameter 'kernel'");
            throw std::runtime_error("Missing required parameter 'kernel'");
        }
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


