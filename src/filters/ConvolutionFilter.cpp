#include <dmxdenoiser/Aov.hpp>
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

        auto kernel_param = params.getSingleParam<Kernel2D>("kernel");
        if (kernel_param)
            kernel.set(*kernel_param);
        else
            throw std::runtime_error("Missing required parameter 'kernel'");
    };

    void ConvolutionFilter::apply(DMXImage& img) const
    {
        convolve2D(img, kernel, frames, layers, filterAlpha);
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


