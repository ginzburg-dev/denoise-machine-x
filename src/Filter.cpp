#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Filter.hpp>

namespace dmxdenoiser
{

    std::vector<int> Filter::resolveFrameIndices(
        const DMXImage& input,
        const std::vector<int>& frames
    ) const {
        std::vector<int> frameIndices;
        // If no specific frames were set, process all frames by default.
        if (frames.empty())
        {
            for (int i = 0; i < input.numFrames(); ++i) // Add all frames
                    frameIndices.push_back(i);
        } else {
            for (int i = 0; i < frames.size(); ++i)
            {
                int requestedFrame = frames[i];
                if(requestedFrame < input.numFrames())
                    frameIndices.push_back(requestedFrame);
                else
                    DMX_LOG_WARNING(Name(),
                        "Filter::apply(): requested frame ", requestedFrame,
                        " out of range for input; skipping");
            }
        }
        return frameIndices;
    }

    std::vector<int> Filter::resolveLayerIndices(
            const DMXImage& input,
            const std::vector<std::string>& layers
    ) const {
        std::vector<int> layerIndices;
        // If no specific layers were set, process by default.   
        if (layers.empty()) {
            layerIndices = input.getFilteringLayersIndices();
        } else {
            for (const auto& layer : layers)
            {
                if (input.hasLayer(layer))
                    layerIndices.push_back(input.getLayerIndex(layer));
                else
                    DMX_LOG_WARNING(Name(), 
                        "Filter::apply(): requested layer '", layer, "' not found; skipping");
            }
        }
        return layerIndices;
    }

    void Filter::apply(
        const DMXImage& in, 
        DMXImage& out, 
        const std::vector<std::string>& layers, 
        const std::vector<int>& frames
    ) const
    {
        std::vector<int> frameIndices = resolveFrameIndices(in, frames);
        std::vector<int> layerIndices = resolveLayerIndices(in, layers);

        if (frameIndices.empty() || layerIndices.empty()) {
            DMX_LOG_WARNING(Name(),
                "Filter::apply(): no valid frames or layers to process; skipping");
            return;
        }

        if (m_backend == Backend::CPU) {
            this->runFilterCPU(in, out, layerIndices, frameIndices);
        } else if (m_backend == Backend::GPU) {
            this->runFilterGPU(in, out, layerIndices, frameIndices);
        }  else if (m_backend == Backend::METAL) {
            this->runFilterMETAL(in, out, layerIndices, frameIndices);
        } else {
            DMX_LOG_ERROR(Name(),
                "apply(): Unsupported backend: ", dmxdenoiser::ToString(m_backend));
            throw std::runtime_error("Unsupported backend");
        }
    }

    void Filter::apply(
        DMXImage& inOut,
        const std::vector<std::string>& layers,
        const std::vector<int>& frames
    ) const
    {
        DMXImage tmp{ inOut };
        apply(inOut, tmp, layers, frames);
        inOut = std::move(tmp);
    }

    void Filter::resetParams() {
        m_strength = 1.0f;
        m_filterAlpha = false;
        m_backend = Backend::CPU;
        m_backendResource = {};
        m_filterInfo.clear();
    };

    void Filter::setParams(const ParamDictionary& params) {
        if (auto v = params.getSingleParam<float>("strength")) {
            m_strength = *v;
            m_filterInfo += "\tstrength (set) = " + std::to_string(m_strength) + "\n";
        }
        else
        {
            m_filterInfo += "\tstrength (default) = " + std::to_string(m_strength) + "\n";
            DMX_LOG_TRACE(Name(), "setParams(): 'strength' parameter not set, using default: ", m_strength);
        }

        if (auto v = params.getSingleParam<bool>("filterAlpha"))
        {
            m_filterAlpha = *v;
            m_filterInfo += "\tfilterAlpha (set) = " + std::string(m_filterAlpha ? "true" : "false") + "\n";
        }
        else
        {
            m_filterInfo += "\tfilterAlpha (default) = " + std::string(m_filterAlpha ? "true" : "false") + "\n";
            DMX_LOG_TRACE(Name(), 
                "setParams(): 'filterAlpha' parameter not set, using default: ", m_filterAlpha);
        }

        if (auto v = params.getSingleParam<Backend>("backend"))
        {
            m_backend = *v;
            m_filterInfo += "\tbackend (set) = " + dmxdenoiser::ToString(m_backend) + "\n";
        }
        else
        {
            m_filterInfo += "\tbackend (default) = " + dmxdenoiser::ToString(m_backend) + "\n";
            DMX_LOG_TRACE(Name(), 
                "setParams(): 'backend' parameter not set, using default: ", dmxdenoiser::ToString(m_backend));
        }

        if (auto v = params.getSingleParam<BackendResource>("backendResource"))
        {
            m_backendResource = *v;
            m_filterInfo += "\tbackendResource (set) = \n" + m_backendResource.ToString(10) + "\n";
        }
        else
        {
            m_filterInfo += "\tbackendResource (default) = \n" + m_backendResource.ToString(10) + "\n";
            DMX_LOG_TRACE(Name(), 
                "setParams(): 'backendResource' parameter not set, using default: \n", 
                m_backendResource.ToString(10));
        }
    }

    std::string Filter::ToString() const
    {
        return std::string(Name()) + ": \n" + m_filterInfo;
    };

    void Filter::runFilterGPU(
        const DMXImage& input,
        DMXImage& output,
        const std::vector<int>& layers,
        const std::vector<int>& frames
    ) const {
        // Default: fallback
        DMX_LOG_WARNING(Name(),
                "GPU backend not implemented; falling back to CPU implementation");
        runFilterCPU(input, output, layers, frames);
    }

    void Filter::runFilterMETAL(
        const DMXImage& input,
        DMXImage& output,
        const std::vector<int>& layers,
        const std::vector<int>& frames
    ) const {
        // Default: fallback
        DMX_LOG_WARNING(Name(),
                "GPU backend not implemented; falling back to CPU implementation");
        runFilterCPU(input, output, layers, frames);
    }

} // namespace dmxdenoiser
