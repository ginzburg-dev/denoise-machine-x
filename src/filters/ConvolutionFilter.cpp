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
        auto kernel_param = params.getArrayParam<float>("kernel");
        if (kernel_param)
            kernel.set(*kernel_param);
        else
            throw std::runtime_error("Missing required parameter 'kernel'");
    };

    void ConvolutionFilter::apply(DMXImage& img) const
    {
        std::cout << "Filtering DMXImage... \n";
    };

    std::string ConvolutionFilter::ToString() const
    {
        return "ConvolutionFilter: " + kernel.ToString();
    };

    REGISTER_FILTER(ConvolutionFilter)

} // namespace dmxdenoiser


