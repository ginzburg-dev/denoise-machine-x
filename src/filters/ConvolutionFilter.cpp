#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/filters/ConvolutionFilter.hpp>
#include <dmxdenoiser/Logger.hpp>

#include <optional>
#include <iostream>
#include <memory>
#include <vector>


namespace dmxdenoiser
{

    std::unique_ptr<FilterParams> ConvolutionFilterParams::clone() const {
        return std::make_unique<ConvolutionFilterParams>(*this);
    };
    
    ConvolutionFilter::ConvolutionFilter() {
        m_params = std::make_unique<ConvolutionFilterParams>();
    }

    ConvolutionFilterParams& ConvolutionFilter::params() {
        return static_cast<ConvolutionFilterParams&>(*m_params);
    }

    const ConvolutionFilterParams& ConvolutionFilter::params() const {
        return static_cast<const ConvolutionFilterParams&>(*m_params);
    }

    void ConvolutionFilter::setParams(const ParamDictionary& paramDict)
    {
        resetParams();

        if (auto v = paramDict.getSingleParam<float>("strength")) params().strength = *v;
        if (auto v = paramDict.getArrayParam<int>("frames")) params().frames = *v;
        if (auto v = paramDict.getArrayParam<std::string>("layers")) params().layers = *v;
        if (auto v = paramDict.getSingleParam<bool>("filterAlpha")) params().filterAlpha = *v;
        if (auto v = paramDict.getSingleParam<std::string>("backend")) params().backend = parseBackend(*v);
        if (auto v = paramDict.getSingleParam<BackendResource>("backendResource")) params().backendResource = *v;

        if (auto v = paramDict.getSingleParam<Kernel2D>("kernel"))
        {
            params().kernel.set(*v);
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
        if (params().backend == Backend::CPU)
        {
            convolve2D(in, out, params().kernel, params().frames, params().layers, params().filterAlpha);
        }
    };

    std::string ConvolutionFilter::ToString() const
    {
        // IN PROGRESS
        return "ConvolutionFilter: \n" + params().kernel.ToString(4);
    };

    void ConvolutionFilter::resetParams() {
        params().strength = 1.0f; params().filterAlpha = false;
        params().frames.clear(); params().layers.clear();
        params().backend = Backend::CPU; params().backendResource = {};
        params().kernel.clear();
    }

    ConvolutionFilter::~ConvolutionFilter() = default;

    REGISTER_FILTER(ConvolutionFilter)

} // namespace dmxdenoiser


