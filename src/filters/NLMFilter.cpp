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
#include <cmath>
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
        
        if (auto v = params.getSingleParam<int>("radius")) {
            m_radius = *v;
            paramsInfo += "     radius (set) = " + std::to_string(m_radius) + "\n";
        }
        else
        {
            paramsInfo += "     radius (default) = \n" + std::to_string(m_radius) + "\n";
            DMX_LOG_TRACE("NLMFilter", "setParams(): 'radius' parameter not set, using default: \n", std::to_string(m_radius));
        }

        if (auto v = params.getSingleParam<int>("patchRadius")) {
            m_patchRadius = *v;
            paramsInfo += "     patchRadius (set) = " + std::to_string(m_patchRadius) + "\n";
        }
        else
        {
            paramsInfo += "     patchRadius (default) = \n" + std::to_string(m_patchRadius) + "\n";
            DMX_LOG_TRACE("NLMFilter", "setParams(): 'patchRadius' parameter not set, using default: \n", std::to_string(m_patchRadius));
        }

        if (auto v = params.getSingleParam<float>("sigmaBeauty")) {
            m_sigmaBeauty = *v;
            paramsInfo += "     sigmaBeauty (set) = " + std::to_string(m_sigmaBeauty) + "\n";
        }
        else
        {
            paramsInfo += "     sigmaBeauty (default) = \n" + std::to_string(m_sigmaBeauty) + "\n";
            DMX_LOG_TRACE("NLMFilter", "setParams(): 'sigmaBeauty' parameter not set, using default: \n", std::to_string(m_sigmaBeauty));
        }

        if (auto v = params.getSingleParam<float>("sigmaAlbedo")) {
            m_sigmaAlbedo = *v;
            paramsInfo += "     sigmaAlbedo (set) = " + std::to_string(m_sigmaAlbedo) + "\n";
        }
        else
        {
            paramsInfo += "     sigmaAlbedo (default) = \n" + std::to_string(m_sigmaAlbedo) + "\n";
            DMX_LOG_TRACE("NLMFilter", "setParams(): 'sigmaAlbedo' parameter not set, using default: \n", std::to_string(m_sigmaAlbedo));
        }

        if (auto v = params.getSingleParam<float>("sigmaNormal")) {
            m_sigmaNormal = *v;
            paramsInfo += "     sigmaNormal (set) = " + std::to_string(m_sigmaNormal) + "\n";
        }
        else
        {
            paramsInfo += "     sigmaNormal (default) = \n" + std::to_string(m_sigmaNormal) + "\n";
            DMX_LOG_TRACE("NLMFilter", "setParams(): 'sigmaNormal' parameter not set, using default: \n", std::to_string(m_sigmaNormal));
        }

        if (auto v = params.getSingleParam<float>("sigmaDepth")) {
            m_sigmaDepth = *v;
            paramsInfo += "     sigmaDepth (set) = " + std::to_string(m_sigmaDepth) + "\n";
        }
        else
        {
            paramsInfo += "     sigmaDepth (default) = \n" + std::to_string(m_sigmaDepth) + "\n";
            DMX_LOG_TRACE("NLMFilter", "setParams(): 'sigmaDepth' parameter not set, using default: \n", std::to_string(m_sigmaDepth));
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
        bool isAlbedo = input.hasLayer("albedo");
        bool isNormal = input.hasLayer("normal");
        bool isDepth = input.hasLayer("depth");

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
        
        for(int frameIdx = 0; frameIdx < framesIndices.size(); ++frameIdx)
        {
            int frame = framesIndices[frameIdx];
            int beautyLayerIndex = input.getLayerIndex("beauty");
            int albedoLayerIndex = -1;
            int normalLayerIndex = -1;
            int depthLayerIndex = -1;
            if (isAlbedo) albedoLayerIndex = input.getLayerIndex("albedo");
            if (isNormal) normalLayerIndex = input.getLayerIndex("normal");
            if (isDepth) depthLayerIndex = input.getLayerIndex("depth");

            float eps = 1e-12f;
            auto sqr = [](float v){ return v*v; };

            parallelFor(0, to_i64(height), [&](std::int64_t y) {
                for(std::int64_t x = 0; x < to_i64(width); ++x)
                {
                    PixelRGBA orig = input.get(to_int(x), to_int(y), frame, beautyLayerIndex);
                    PixelRGBA sum = {0.0f, 0.0f, 0.0f, 0.0f};
                    float weightSum = 0.f;
                    for(int ky = -m_radius; ky <= m_radius; ++ky)
                        for(int kx = -m_radius; kx <= m_radius; ++kx)
                        {
                            int px = std::clamp(to_int(x) + kx, 0, width - 1);
                            int py = std::clamp(to_int(y) + ky, 0, height - 1);

                            PixelRGBA pixBeautyP = input.get(px, py, frame, beautyLayerIndex);

                            float ssdBeauty = 0.f;
                            float ssdAlbedo = 0.f;
                            float ssdNormal = 0.f;
                            float ssdDepth = 0.f;

                            for(int jy = -m_patchRadius; jy <= m_patchRadius; ++jy)
                                for(int jx = -m_patchRadius; jx <= m_patchRadius; ++jx) 
                                {
                                    int poxj = std::clamp(to_int(x) + jx, 0, width - 1);
                                    int poyj = std::clamp(to_int(y) + jy, 0, height - 1);
                                    int pxj = std::clamp(px + jx, 0, width - 1);
                                    int pyj = std::clamp(py + jy, 0, height - 1);

                                    PixelRGBA OjB = input.get(poxj, poyj, frame, beautyLayerIndex);
                                    PixelRGBA PjB = input.get(pxj, pyj, frame, beautyLayerIndex);
                                    float dbr = OjB.r - PjB.r;
                                    float dbg = OjB.g - PjB.g;
                                    float dbb = OjB.b - PjB.b;
                                    ssdBeauty += dbr*dbr + dbg*dbg + dbb*dbb;

                                    if (isAlbedo){
                                        PixelRGBA OjA = input.get(poxj, poyj, frame, albedoLayerIndex);
                                        PixelRGBA PjA = input.get(pxj, pyj, frame, albedoLayerIndex);
                                        float dar = OjA.r - PjA.r;
                                        float dag = OjA.g - PjA.g;
                                        float dab = OjA.b - PjA.b;
                                        ssdAlbedo += dar*dar + dag*dag + dab*dab;
                                    }

                                    if (isNormal){
                                        PixelRGBA OjN = input.get(poxj, poyj, frame, normalLayerIndex);
                                        PixelRGBA PjN = input.get(pxj, pyj, frame, normalLayerIndex);
                                        float dnr = OjN.r - PjN.r;
                                        float dng = OjN.g - PjN.g;
                                        float dnb = OjN.b - PjN.b;
                                        ssdNormal += dnr*dnr + dng*dng + dnb*dnb;
                                    }

                                    if (isDepth){
                                        PixelRGBA OjD = input.get(poxj, poyj, frame, depthLayerIndex);
                                        PixelRGBA PjD = input.get(pxj, pyj, frame, depthLayerIndex);
                                        float ddr = OjD.r - PjD.r;
                                        float ddg = OjD.g - PjD.g;
                                        float ddb = OjD.b - PjD.b;
                                        ssdDepth += ddr*ddr + ddg*ddg + ddb*ddb;
                                    }
                                }
                            float w = std::exp(-ssdBeauty/std::max(eps, m_sigmaBeauty*m_sigmaBeauty));
                            if (isAlbedo) w *= std::exp(-ssdAlbedo/std::max(eps, m_sigmaAlbedo*m_sigmaAlbedo));
                            if (isNormal) w *= std::exp(-ssdNormal/std::max(eps, m_sigmaNormal*m_sigmaNormal));
                            if (isDepth) w *= std::exp(-ssdDepth/std::max(eps, m_sigmaDepth*m_sigmaDepth));
                            weightSum += w;
                            sum += w * pixBeautyP;
                        }
                    if (weightSum > 0.0f) {
                        sum /= weightSum;
                    } else {
                        sum = orig;
                    }
                    sum = blendPixels(orig, sum, m_strength, m_filterAlpha);
                    output.at(to_int(x), to_int(y), frame, beautyLayerIndex) = sum;
                }
            }, pool);
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
        if (m_patchRadius == 0) {
            DMX_LOG_ERROR("NLMFilter", "applyFilter(): patchRadius is zero");
            throw std::runtime_error("NLMFilter::applyFilter(): patchRadius is zero");
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
        return "NLMFilter: \n";
    };

    REGISTER_FILTER(NLMFilter)

} // namespace dmxdenoiser


