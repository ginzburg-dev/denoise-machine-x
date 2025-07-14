#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/filters/ExampleFilter.hpp>

namespace dmxdenoiser
{

    void ExampleFilter::setParams(const ParamDictionary& params)
    {
        /* Set all parameters */
    };

    void ExampleFilter::apply(DMXImage& img) const
    {
        /* Process image */
    };

    std::string ExampleFilter::ToString() const
    {
        return "ExampleFilter: kernel=" + std::to_string(kernel) + 
            ", radius=" + std::to_string(radius) + 
            ", sigma=" + utils::floatToString(sigma);
    };

    REGISTER_FILTER(ExampleFilter)

} // namespace dmxdenoiser


