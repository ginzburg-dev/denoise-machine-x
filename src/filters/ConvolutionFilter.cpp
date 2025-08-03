#include <dmxdenoiser/Aov.hpp>
#include <dmxdenoiser/FilterFactory.hpp>
#include <dmxdenoiser/FilterKernels.hpp>
#include <dmxdenoiser/filters/ConvolutionFilter.hpp>

#include <optional>
#include <iostream>
#include <vector>

namespace dmxdenoiser
{   
    
    void ConvolutionFilter::setParams(const ParamDictionary& params)
    {
        resetParams();

        auto kernel_param = params.getSingleParam<Kernel2D>("kernel");
        if (kernel_param)
            kernel.set(*kernel_param);
        else
            throw std::runtime_error("Missing required parameter 'kernel'");

        auto strength_param = params.getSingleParam<float>("strength");
        if (strength_param)
            strength = *strength_param;
        
        auto frames_param = params.getArrayParam<int>("frames");
        if (frames_param)
            frames = *frames_param;

        auto layers_param = params.getArrayParam<std::string>("layers");
        if (layers_param)
            layers = *layers_param;

        auto alpha_param = params.getSingleParam<bool>("filterAlpha");
        if (alpha_param)
            filterAlpha = *alpha_param;
    };

    void ConvolutionFilter::apply(DMXImage& img) const
    {
        convolveSimple(img);
    };

    std::string ConvolutionFilter::ToString() const
    {
        return "ConvolutionFilter: " + kernel.ToString();
    };

    void ConvolutionFilter::resetParams() {
        Filter::resetParams();
        kernel.clear();
    }

    void ConvolutionFilter::convolveSimple(DMXImage& img) const
    {
        int width = img.width();
        int height = img.height();

        int ksize = kernel.size();
        int offset = ksize/2;
        
        std::vector<int> framesIndices;

        // If no specific frames were set, process all frames by default.
        if (frames.empty())
        {
            for (int i = 0; i < img.numFrames(); ++i)
                framesIndices.push_back(i);
        } else {
            framesIndices = frames;
        }

        std::vector<int> layerIndices;

        // If no specific layers were set, process by default.   
        if (layers.empty()) {
            layerIndices = img.getFilteringLayersIndices();
        } else {
            for (const auto& layer : layers)
            {
                if (img.hasLayer(layer))
                    layerIndices.push_back(img.getLayerIndex(layer));
            }
        }

        std::vector<PixelRGBA> pixelBuffer(width * height * layerIndices.size() * framesIndices.size());

        for(int frameIdx = 0; frameIdx < framesIndices.size(); ++frameIdx)
        {
            int frame = framesIndices[frameIdx];
            for(int layerIdx = 0; layerIdx < layerIndices.size(); ++layerIdx)
            {
                int layer = layerIndices[layerIdx];
                for(int y = 0; y < height; ++y)
                    for(int x = 0; x < width; ++x)
                    {
                        PixelRGBA sum = {0.0f, 0.0f, 0.0f, 0.0f};
                        for(int ky = -offset; ky <= offset; ++ky)
                            for(int kx = -offset; kx <= offset; ++kx)
                            {
                                int px = std::clamp(x + kx, 0, width - 1);
                                int py = std::clamp(y + ky, 0, height - 1);
                                sum += kernel(ky + offset, kx + offset) * img.get(px, py, frame, layer);
                            }
                        if (!filterAlpha)
                            sum.a = img.at(x, y, frame, layer).a;
                        std::size_t pixelIdx =  ((frameIdx*layerIndices.size() + layerIdx)*height + y)*width + x;
                        pixelBuffer[pixelIdx] = sum;
                    }
            }
        }

        for(int frameIdx = 0; frameIdx < framesIndices.size(); ++frameIdx)
        {
            int frame = framesIndices[frameIdx];
            for(int layerIdx = 0; layerIdx < layerIndices.size(); ++layerIdx)
            {
                int layer = layerIndices[layerIdx];
                for(int y = 0; y < height; ++y)
                    for(int x = 0; x < width; ++x)
                        {
                            std::size_t pixelIdx = ((frameIdx*layerIndices.size() + layerIdx)*height + y)*width + x;
                            img.at(x, y, frame, layer) = pixelBuffer[pixelIdx];
                        }
            }
        }

    }

    REGISTER_FILTER(ConvolutionFilter)

} // namespace dmxdenoiser


