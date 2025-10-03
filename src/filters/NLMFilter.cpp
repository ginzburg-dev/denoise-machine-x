#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/StringConversions.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/filters/NLMFilter.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/Parallel.hpp>
#include <dmxdenoiser/utils/NumericUtils.hpp>

#if DMX_ENABLE_CUDA
    #include <dmxdenoiser/NLMFilterCUDA.cu>
#endif

#include <optional>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace dmxdenoiser
{

    void NLMFilter::setParams(const ParamDictionary& params)
    {
        resetParams();

        std::string paramsInfo{};

        if (auto v = params.getSingleParam<float>("strength")) {
            m_strength = *v;
            paramsInfo += "     strength (set) = " + std::to_string(m_strength) + "\n";
        }
        else
        {
            paramsInfo += "     strength (default) = " + std::to_string(m_strength) + "\n";
            DMX_LOG_TRACE("NLMFilter", "setParams(): 'strength' parameter not set, using default: ", m_strength);
        }
        
        if (auto v = params.getArrayParam<int>("frames"))
        {
            m_frames = *v;
            paramsInfo += "     frames (set) = " + joinVector(m_frames, ", ", "[","]", "all") + "\n";
        }
        else
        {
            paramsInfo += "     frames (default) = " + joinVector(m_frames, ", ", "[","]", "all") + "\n";
            DMX_LOG_TRACE("NLMFilter", "setParams(): 'frames' parameter not set, using 'default' value: ",
                joinVector(m_frames, ", ", "[","]", "all"));
        }
        
        if (auto v = params.getArrayParam<std::string>("layers")) 
        {
            m_layers = *v;
            paramsInfo += "     layers (set) = " + joinVector(m_layers, ", ", "[","]", "all") + "\n";
        }
        else
        {
            paramsInfo += "     layers (default)  = " + joinVector(m_layers, ", ", "[","]", "all") + "\n";
            DMX_LOG_TRACE("NLMFilter", 
                "setParams(): 'layers' parameter not set, using default: ", joinVector(m_layers, ", ", "[","]", "all"));
        }

        if (auto v = params.getSingleParam<bool>("filterAlpha"))
        {
            m_filterAlpha = *v;
            paramsInfo += "     filterAlpha (set) = " + std::string(m_filterAlpha ? "true" : "false") + "\n";
        }
        else
        {
            paramsInfo += "     filterAlpha (default) = " + std::string(m_filterAlpha ? "true" : "false") + "\n";
            DMX_LOG_TRACE("NLMFilter", 
                "setParams(): 'filterAlpha' parameter not set, using default: ", m_filterAlpha);
        }

        if (auto v = params.getSingleParam<Backend>("backend"))
        {
            m_backend = *v;
            paramsInfo += "     backend (set) = " + dmxdenoiser::ToString(m_backend) + "\n";
        }
        else
        {
            paramsInfo += "     backend (default) = " + dmxdenoiser::ToString(m_backend) + "\n";
            DMX_LOG_TRACE("NLMFilter", 
                "setParams(): 'backend' parameter not set, using default: ", dmxdenoiser::ToString(m_backend));
        }

        if (auto v = params.getSingleParam<BackendResource>("backendResource"))
        {
            m_backendResource = *v;
            paramsInfo += "     backendResource (set) = \n" + m_backendResource.ToString(10) + "\n";
        }
        else
        {
            paramsInfo += "     backendResource (default) = \n" + m_backendResource.ToString(10) + "\n";
            DMX_LOG_TRACE("NLMFilter", "setParams(): 'backendResource' parameter not set, using default: \n", m_backendResource.ToString(10));
        }
        
        if (auto v = params.getSingleParam<Kernel2D>("kernel")) { 
            m_kernel.set(*v);
            paramsInfo += "     kernel (set) = " + m_kernel.ToString() + "\n";
        }
        else
        {
            DMX_LOG_ERROR("NLMFilter", "setParams(): Missing required parameter 'kernel'");
            throw std::runtime_error("NLMFilter::setParams(): Missing required parameter 'kernel'");
        }
        DMX_LOG_INFO("NLMFilter", "Setup filter settings:\nParameters:\n", paramsInfo);
    };

    void NLMFilter::runFilterCPU(const DMXImage& input, DMXImage& output) const
    {
        ThreadPool* pool = m_backendResource.threadPool;
        if(!pool)
            DMX_LOG_WARNING("NLMFilter", "runFilterCPU(): no ThreadPool available; running single-threaded");

        int width = input.width();
        int height = input.height();
        int ksize = m_kernel.size();
        int offset = ksize/2;

        std::vector<int> framesIndices;
        // If no specific frames were set, process all frames by default.
        if (m_frames.empty())
        {
            for (int i = 0; i < input.numFrames(); ++i) // Add all frames
                    framesIndices.push_back(i);
        } else {
            for (int i = 0; i < m_frames.size(); ++i)
            {
                int requestedFrame = m_frames[i];
                if(requestedFrame < input.numFrames())
                    framesIndices.push_back(requestedFrame);
                else
                    DMX_LOG_WARNING("NLMFilter", "setParams(): requested frame ", 
                        requestedFrame, " not found; skipping");
            }
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
                else
                    DMX_LOG_WARNING("NLMFilter", "setParams(): requested layer '", layer, "' not found; skipping");
            }
        }

        for(int frameIdx = 0; frameIdx < framesIndices.size(); ++frameIdx)
        {
            int frame = framesIndices[frameIdx];
            for(int layerIdx = 0; layerIdx < layerIndices.size(); ++layerIdx)
            {
                int layer = layerIndices[layerIdx];
                parallelFor(0, to_i64(height), [&](std::int64_t y) {
                    for(std::int64_t x = 0; x < to_i64(width); ++x)
                    {
                        PixelRGBA orig = input.get(to_int(x), to_int(y), frame, layer);
                        PixelRGBA sum = {0.0f, 0.0f, 0.0f, 0.0f};
                        for(int ky = -offset; ky <= offset; ++ky)
                            for(int kx = -offset; kx <= offset; ++kx)
                            {
                                int px = std::clamp(to_int(x) + kx, 0, width - 1);
                                int py = std::clamp(to_int(y) + ky, 0, height - 1);
                                sum += m_kernel(ky + offset, kx + offset) * input.get(px, py, frame, layer);
                            }
                        sum = blendPixels(orig, sum, m_strength, m_filterAlpha);
                        output.at(to_int(x), to_int(y), frame, layer) = sum;
                    }
                }, pool);
            }
        }
    }

    void NLMFilter::runFilterGPU(const DMXImage& input, DMXImage& output) const
    {
        #if DMX_ENABLE_CUDA
            // GPU logic
        #else
            DMX_LOG_ERROR("NLMFilter", "runFilterGPU(): no CUDA build");
            throw std::runtime_error("runFilterGPU(): no CUDA build");
        #endif
    }

    void NLMFilter::runFilterMETAL(const DMXImage& input, DMXImage& output) const
    {
        //#if DMX_ENABLE_METAL
        //    // METAL logic
        //#else
            DMX_LOG_ERROR("NLMFilter", "runFilterMETAL(): no METAL build");
            throw std::runtime_error("runFilterMETAL(): no METAL build");
        //#endif
    }
    
    void NLMFilter::applyFilter(const DMXImage& in, DMXImage& out) const
    {
        if (m_kernel.size() == 0) {
            DMX_LOG_ERROR("NLMFilter", "applyFilter(): Kernel is empty, size=0x0");
            throw std::runtime_error("NLMFilter::applyFilter(): Kernel is empty, size=0x0");
        }

        if (m_backend == Backend::CPU) {
            this->runFilterCPU(in, out);
        } else if (m_backend == Backend::GPU) {
            this->runFilterGPU(in, out);
        }  else if (m_backend == Backend::METAL) {
            this->runFilterMETAL(in, out);
        }
    };

    std::string NLMFilter::ToString() const
    {
        // IN PROGRESS
        return "NLMFilter: \n" + m_kernel.ToString(4);
    };

    REGISTER_FILTER(NLMFilter)

} // namespace dmxdenoiser


