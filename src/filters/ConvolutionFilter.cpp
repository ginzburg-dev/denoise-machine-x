#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/filters/ConvolutionFilter.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/Parallel.hpp>

#include <optional>
#include <iostream>
#include <memory>
#include <vector>

namespace dmxdenoiser
{

    void ConvolutionFilter::setParams(const ParamDictionary& params)
    {
        resetParams();

        if (auto v = params.getSingleParam<float>("strength"))
            m_strength = *v;
        else 
            DMX_LOG_INFO("ConvolutionFilter", "setParams(): 'strength' parameter not set, using default: ", m_strength);
        
        if (auto v = params.getArrayParam<int>("frames")) 
            m_frames = *v;
        else
            DMX_LOG_INFO("ConvolutionFilter", "setParams(): 'frames' parameter not set, using default: {}");
        
        if (auto v = params.getArrayParam<std::string>("layers")) 
            m_layers = *v;
        else
            DMX_LOG_INFO("ConvolutionFilter", "setParams(): 'layers' parameter not set, using default: {}");

        if (auto v = params.getSingleParam<bool>("filterAlpha")) 
            m_filterAlpha = *v;
        else
            DMX_LOG_INFO("ConvolutionFilter", 
                "setParams(): 'filterAlpha' parameter not set, using default: ", m_filterAlpha);

        if (auto v = params.getSingleParam<Backend>("backend")) 
            m_backend = *v;
        else
            DMX_LOG_INFO("ConvolutionFilter", 
                "setParams(): 'backend' parameter not set, using default: ", dmxdenoiser::ToString(m_backend));

        if (auto v = params.getSingleParam<BackendResource>("backendResource")) 
            m_backendResource = *v;
        else
            DMX_LOG_INFO("ConvolutionFilter", "setParams(): 'backendResource' parameter not set, using default: {}");
        
        if (auto v = params.getSingleParam<Kernel2D>("kernel")) { 
            m_kernel.set(*v);
        }
        else
        {
            DMX_LOG_ERROR("ConvolutionFilter", "setParams(): Missing required parameter 'kernel'");
            throw std::runtime_error("ConvolutionFilter::setParams(): Missing required parameter 'kernel'");
        }
    };

    void ConvolutionFilter::convolveCPU(const DMXImage& input, DMXImage& output) const
    {
        ThreadPool* pool = m_backendResource.threadPool;
        if(!pool)
            DMX_LOG_INFO("ConvolutionFilter", "convolveCPU(): no ThreadPool available; runing single-threaded");
        int width = input.width();
        int height = input.height();
        int ksize = m_kernel.size();
        int offset = ksize/2;

        std::vector<int> framesIndices;
        // If no specific frames were set, process all frames by default.
        if (m_frames.empty())
        {
            for (int i = 0; i < input.numFrames(); ++i)
                framesIndices.push_back(i);
        } else {
            framesIndices = m_frames;
        }

        std::vector<int> layerIndices;
        // If no specific layers were set, process by default.   
        if (m_layers.empty()) {
            layerIndices = input.getFilteringLayersIndices();
        } else {
            for (const auto& layer : m_layers)
            {
                if (input.hasLayer(layer))
                    layerIndices.push_back(input.getLayerIndex(layer));
            }
        }

        for(int frameIdx = 0; frameIdx < framesIndices.size(); ++frameIdx)
        {
            int frame = framesIndices[frameIdx];
            for(int layerIdx = 0; layerIdx < layerIndices.size(); ++layerIdx)
            {
                int layer = layerIndices[layerIdx];
                parallelFor(0, height, [&](int y){
                    for(int x = 0; x < width; ++x)
                    {
                        PixelRGBA orig = input.get(x, y, frame, layer);
                        PixelRGBA sum = {0.0f, 0.0f, 0.0f, 0.0f};
                        for(int ky = -offset; ky <= offset; ++ky)
                            for(int kx = -offset; kx <= offset; ++kx)
                            {
                                int px = std::clamp(x + kx, 0, width - 1);
                                int py = std::clamp(y + ky, 0, height - 1);
                                sum += m_kernel(ky + offset, kx + offset) * input.get(px, py, frame, layer);
                            }
                        sum = blendPixels(orig, sum, m_strength, m_filterAlpha);
                        output.at(x, y, frame, layer) = sum;
                    }
                }, pool);
                /*
                for(int y = 0; y < height; ++y)
                    for(int x = 0; x < width; ++x)
                    {
                        PixelRGBA orig = input.get(x, y, frame, layer);
                        PixelRGBA sum = {0.0f, 0.0f, 0.0f, 0.0f};
                        for(int ky = -offset; ky <= offset; ++ky)
                            for(int kx = -offset; kx <= offset; ++kx)
                            {
                                int px = std::clamp(x + kx, 0, width - 1);
                                int py = std::clamp(y + ky, 0, height - 1);
                                sum += kernel(ky + offset, kx + offset) * input.get(px, py, frame, layer);
                            }
                        sum = blendPixels(orig, sum, strength, filterAlpha);
                        output.at(x, y, frame, layer) = sum;
                    }
                */
            }
        }
    }

    void ConvolutionFilter::convolveGPU(const DMXImage& input, DMXImage& output) const
    {
        // IN PROGRESS
        output = input;
    }

    void ConvolutionFilter::convolveMETAL(const DMXImage& input, DMXImage& output) const
    {
        // IN PROGRESS
        output = input;
    }
    
    void ConvolutionFilter::applyFilter(const DMXImage& in, DMXImage& out) const
    {
        if (m_kernel.size() == 0) {
            DMX_LOG_ERROR("ConvolutionFilter::applyFilter()", "Kernel is empty, size={}x{}");
            throw std::runtime_error("Kernel is empty, size={}x{}");
        }

        if (m_backend == Backend::CPU) {
            this->convolveCPU(in, out);
        } else if (m_backend == Backend::GPU) {
            this->convolveGPU(in, out);
        }  else if (m_backend == Backend::METAL) {
            this->convolveMETAL(in, out);
        }
    };

    std::string ConvolutionFilter::ToString() const
    {
        // IN PROGRESS
        return "ConvolutionFilter: \n" + m_kernel.ToString(4);
    };

    REGISTER_FILTER(ConvolutionFilter)

} // namespace dmxdenoiser


