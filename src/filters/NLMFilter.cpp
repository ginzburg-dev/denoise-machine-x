#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/StringConversions.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/filters/NLMFilter.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/Parallel.hpp>
#include <dmxdenoiser/utils/NumericUtils.hpp>

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
        Filter::setParams(params);

        if (auto v = params.getSingleParam<int>("radius")) {
            m_radius = *v;
            m_filterInfo += "\tradius (set) = " + std::to_string(m_radius) + "\n";
        }
        else
        {
            m_filterInfo += "\tradius (default) = \n" + std::to_string(m_radius) + "\n";
            DMX_LOG_TRACE(Name(), "setParams(): 'radius' parameter not set, using default: \n",
                std::to_string(m_radius));
        }

        if (auto v = params.getSingleParam<int>("patchRadius")) {
            m_patchRadius = *v;
            m_filterInfo += "\tpatchRadius (set) = " + std::to_string(m_patchRadius) + "\n";
        }
        else
        {
            m_filterInfo += "\tpatchRadius (default) = \n" + std::to_string(m_patchRadius) + "\n";
            DMX_LOG_TRACE(Name(), "setParams(): 'patchRadius' parameter not set, using default: \n", 
                std::to_string(m_patchRadius));
        }

        if (auto v = params.getSingleParam<float>("sigmaBeauty")) {
            m_sigmaBeauty = *v;
            m_filterInfo += "\tsigmaBeauty (set) = " + std::to_string(m_sigmaBeauty) + "\n";
        }
        else
        {
            m_filterInfo += "\tsigmaBeauty (default) = \n" + std::to_string(m_sigmaBeauty) + "\n";
            DMX_LOG_TRACE(Name(), "setParams(): 'sigmaBeauty' parameter not set, using default: \n",
                std::to_string(m_sigmaBeauty));
        }

        if (auto v = params.getSingleParam<float>("sigmaAlbedo")) {
            m_sigmaAlbedo = *v;
            m_filterInfo += "\tsigmaAlbedo (set) = " + std::to_string(m_sigmaAlbedo) + "\n";
        }
        else
        {
            m_filterInfo += "\tsigmaAlbedo (default) = \n" + std::to_string(m_sigmaAlbedo) + "\n";
            DMX_LOG_TRACE(Name(), "setParams(): 'sigmaAlbedo' parameter not set, using default: \n",
                std::to_string(m_sigmaAlbedo));
        }

        if (auto v = params.getSingleParam<float>("sigmaNormal")) {
            m_sigmaNormal = *v;
            m_filterInfo += "\tsigmaNormal (set) = " + std::to_string(m_sigmaNormal) + "\n";
        }
        else
        {
            m_filterInfo += "\tsigmaNormal (default) = \n" + std::to_string(m_sigmaNormal) + "\n";
            DMX_LOG_TRACE(Name(), "setParams(): 'sigmaNormal' parameter not set, using default: \n", 
                std::to_string(m_sigmaNormal));
        }

        if (auto v = params.getSingleParam<float>("sigmaDepth")) {
            m_sigmaDepth = *v;
            m_filterInfo += "\tsigmaDepth (set) = " + std::to_string(m_sigmaDepth) + "\n";
        }
        else
        {
            m_filterInfo += "\tsigmaDepth (default) = \n" + std::to_string(m_sigmaDepth) + "\n";
            DMX_LOG_TRACE(Name(), "setParams(): 'sigmaDepth' parameter not set, using default: \n", 
                std::to_string(m_sigmaDepth));
        }

        DMX_LOG_DEBUG(Name(), "Setup filter settings:\nParameters:\n", ToString());
    };

    void NLMFilter::resetParams() { 
        Filter::resetParams();
        m_radius = 4;
        m_patchRadius = 3;
        m_sigmaBeauty = 1.f;
        m_sigmaAlbedo = 1.f;
        m_sigmaNormal = 1.f;
        m_sigmaDepth = 1.f;
    };

    void NLMFilter::runFilterCPU(
            const DMXImage& input,
            DMXImage& output,
            const std::vector<int>& layers,
            const std::vector<int>& frames
    ) const
    {
        ThreadPool* pool = m_backendResource.threadPool;
        if(!pool)
            DMX_LOG_WARNING(Name(), 
                "runFilterCPU(): no ThreadPool available; running single-threaded");

        float eps = 1e-12f;

        int width = input.width();
        int height = input.height();
        bool isAlbedo = input.hasLayer("albedo");
        bool isNormal = input.hasLayer("normal");
        bool isDepth = input.hasLayer("depth");

        int beautyLayerIndex = input.getLayerIndex("beauty");
        int albedoLayerIndex = -1;
        int normalLayerIndex = -1;
        int depthLayerIndex = -1;
        if (isAlbedo) albedoLayerIndex = input.getLayerIndex("albedo");
        if (isNormal) normalLayerIndex = input.getLayerIndex("normal");
        if (isDepth) depthLayerIndex = input.getLayerIndex("depth");

        for(int frameIdx = 0; frameIdx < frames.size(); ++frameIdx)
        {
            int frame = frames[frameIdx];
            parallelFor(0, to_i64(height), [&](std::int64_t y) {
                for(std::int64_t x = 0; x < to_i64(width); ++x)
                {
                    PixelRGBA orig = input.get(to_int(x), to_int(y), frame, beautyLayerIndex);

                    std::vector<PixelRGBA> origPixByLayers(layers.size()); // Vector with layers RGBA of origin pixel
                    for(std::size_t i = 0; i < layers.size(); ++i)
                        origPixByLayers[i] = input.get(to_int(x), to_int(y), frame, layers[i]);

                    std::vector<PixelRGBA> sum(layers.size(), {0.0f, 0.0f, 0.0f, 0.0f});

                    float weightSum = 0.f;
                    for(int ky = -m_radius; ky <= m_radius; ++ky)
                        for(int kx = -m_radius; kx <= m_radius; ++kx)
                        {
                            int px = std::clamp(to_int(x) + kx, 0, width - 1);
                            int py = std::clamp(to_int(y) + ky, 0, height - 1);

                            PixelRGBA pixBeautyP = input.get(px, py, frame, beautyLayerIndex);
                            std::vector<PixelRGBA> pPixByLayers(layers.size());
                            for(std::size_t i = 0; i < layers.size(); ++i)
                                pPixByLayers[i] = input.get(px, py, frame, layers[i]);

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

                            // Sum weighted values for all layers
                            for(std::size_t i = 0; i < pPixByLayers.size(); ++i)
                                sum[i] += w * pPixByLayers[i];
                    }
                    for(std::size_t i = 0; i < layers.size(); ++i) {
                        if (weightSum > 0.0f) {
                            sum[i] /= weightSum;
                        } else {
                            sum[i] = origPixByLayers[i];
                        }
                        sum[i] = blendPixels(origPixByLayers[i], sum[i], m_strength, m_filterAlpha);
                        output.at(to_int(x), to_int(y), frame, layers[i]) = sum[i];
                    }
                }
            }, pool);
        }
        
    }

    REGISTER_FILTER(NLMFilter)

} // namespace dmxdenoiser


