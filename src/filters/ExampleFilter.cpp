#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/filters/ExampleFilter.hpp>

#include <optional>
#include <iostream>

// ExampleFilter parameters:
// int kernel
// int radius
// float sigma

namespace dmxdenoiser
{

    void ExampleFilter::setParams(const ParamDictionary& params)
    {
        auto kernel_param = params.getSingleParam<int>("kernel");
        if (kernel_param)
            kernel = *kernel_param;
        else
            throw std::runtime_error("Missing required parameter 'kernel'");

        auto radius_param = params.getSingleParam<int>("radius");
        if (radius_param)
            radius = *radius_param;
        else
            throw std::runtime_error("Missing required parameter 'radius'");

        auto sigma_param = params.getSingleParam<float>("sigma");
        if (sigma_param)
            sigma = *sigma_param;
        else
            throw std::runtime_error("Missing required parameter 'sigma'");
    };

    void ExampleFilter::apply(DMXImage& img) const
    {
        std::cout << "Filtering DMXImage... \n";
    };

    std::string ExampleFilter::ToString() const
    {
        return "ExampleFilter: kernel=" + std::to_string(kernel) + 
            ", radius=" + std::to_string(radius) + 
            ", sigma=" + utils::floatToString(sigma);
    };

    REGISTER_FILTER(ExampleFilter)

} // namespace dmxdenoiser


