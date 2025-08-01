#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/filters/BoxFilter.hpp>

#include <optional>
#include <iostream>
#include <vector>

// BoxFilter parameters:
// int kernel

namespace dmxdenoiser
{   
    /*
    void BoxFilter::setParams(const ParamDictionary& params)
    {
        auto kernel_param = params.getSingleParam<int>("kernel");
        if (kernel_param)
            kernel = *kernel_param;
        else
            throw std::runtime_error("Missing required parameter 'kernel'");
    };

    void BoxFilter::apply(
            DMXImage& img, 
            const std::vector<int>& frames, 
            const std::vector<std::string>& layers) const
    {
        std::cout << "Filtering DMXImage... \n";
    };

    std::string BoxFilter::ToString() const
    {
        return "BoxFilter: kernel=" + std::to_string(kernel);
    };

    REGISTER_FILTER(BoxFilter)
    */
} // namespace dmxdenoiser


