#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/StringConversions.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/filters/ConvolutionCUDA.hpp>
#include <dmxdenoiser/filters/ConvolutionFilter.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/Parallel.hpp>
#include <dmxdenoiser/utils/NumericUtils.hpp>

#include <optional>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace dmxdenoiser
{

    void ConvolutionFilter::setParams(const ParamDictionary& params)
    {
        resetParams();
        Filter::setParams(params);
        
        if (auto v = params.getSingleParam<Kernel2D>("kernel")) { 
            m_kernel.set(*v);
            m_filterInfo += "\tkernel (set) = " + m_kernel.ToString() + "\n";
        }
        else
        {
            DMX_LOG_ERROR(Name(), "setParams(): Missing required parameter 'kernel'");
            std::string errorMessage = std::string(Name()) + "::setParams(): Missing required parameter 'kernel'";
            throw std::runtime_error(errorMessage);
        }
        DMX_LOG_DEBUG(Name(), "Setup filter settings:\nParameters:\n", ToString());
    };

    void ConvolutionFilter::runFilterCPU(
        const DMXImage& input,
        DMXImage& output,
        const std::vector<int>& layers,
        const std::vector<int>& frames
    ) const
    {
        ThreadPool* pool = m_backendResource.threadPool;
        if(!pool)
            DMX_LOG_WARNING(Name(), "runFilterCPU(): no ThreadPool available; running single-threaded");

        const int width = input.width();
        const int height = input.height();
        const int ksize = m_kernel.size();
        const int offset = ksize/2;

        for(int frameIdx = 0; frameIdx < frames.size(); ++frameIdx)
        {
            int frame = frames[frameIdx];
            for(int layerIdx = 0; layerIdx < layers.size(); ++layerIdx)
            {
                int layer = layers[layerIdx];
                parallelFor(0, to_i64(height), [&](std::int64_t y) {
                    for(std::int64_t x = 0; x < to_i64(width); ++x)
                    {
                        PixelRGBA orig = input.get(to_int(x), to_int(y), frame, layer);
                        PixelRGBA sum = {0.0f, 0.0f, 0.0f, 0.0f};
                        for(int ky = -offset; ky <= offset; ++ky)
                        {
                            int py = std::clamp(to_int(y) + ky, 0, height - 1);
                            for(int kx = -offset; kx <= offset; ++kx)
                            {
                                int px = std::clamp(to_int(x) + kx, 0, width - 1);
                                sum += m_kernel(ky + offset, kx + offset) * input.get(px, py, frame, layer);
                            }
                        }
                        sum = blendPixels(orig, sum, m_strength, m_filterAlpha);
                        output.at(to_int(x), to_int(y), frame, layer) = sum;
                    }
                }, pool);
            }
        }
    }

    void ConvolutionFilter::runFilterGPU(
        const DMXImage& input,
        DMXImage& output,
        const std::vector<int>& layers,
        const std::vector<int>& frames
    ) const
    {
        #if DMX_ENABLE_CUDA
            int width = input.width();
            int height = input.height();
            int ksize = m_kernel.size();
            convolve2D_CUDA(input, output, framesIndices, layerIndices, m_kernel, m_strength, m_filterAlpha);
        #else
            DMX_LOG_ERROR(Name(), "runFilterGPU(): no CUDA build");
            throw std::runtime_error("runFilterGPU(): no CUDA build");
        #endif
    }

    void ConvolutionFilter::resetParams() { 
        Filter::resetParams();
        m_kernel.clear();
    };
    
    REGISTER_FILTER(ConvolutionFilter)

} // namespace dmxdenoiser


