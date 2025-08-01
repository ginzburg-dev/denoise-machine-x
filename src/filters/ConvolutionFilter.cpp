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
        auto strength_param = params.getSingleParam<float>("strength");
        if (strength_param)
            strength = *strength_param;
        else
            throw std::runtime_error("Missing required parameter 'strength'");
        
        auto frames_param = params.getArrayParam<int>("frames");
        if (frames_param)
            frames = *frames_param;
        else
            throw std::runtime_error("Missing required parameter 'frames");

        auto alpha_param = params.getSingleParam<bool>("filterAlpha");
        if (alpha_param)
            filterAlpha = *alpha_param;
        else
            throw std::runtime_error("Missing required parameter 'filterAlpha");

        auto layers_param = params.getArrayParam<std::string>("layers");
        if (layers_param)
            layers = *layers_param;
        else
            throw std::runtime_error("Missing required parameter 'layers'");
            
        auto kernel_param = params.getArrayParam<float>("kernel");
        if (kernel_param)
            kernel.set(*kernel_param);
        else
            throw std::runtime_error("Missing required parameter 'kernel'");


    };

    void ConvolutionFilter::apply(DMXImage& img) const
    {
        convolveSimple(img);
    };

    std::string ConvolutionFilter::ToString() const
    {
        return "ConvolutionFilter: " + kernel.ToString();
    };

    void ConvolutionFilter::convolveSimple(DMXImage& img) const
    {
        int width = img.width();
        int height = img.height();

        int ksize = kernel.size();
        int offset = ksize/2;

        auto layerIndices = img.getFilteringLayersIndices();

        std::vector<PixelRGBA> pixelBuffer(width * height * layerIndices.size() * frames.size());

        for(int frameIdx = 0; frameIdx < frames.size(); ++frameIdx)
        {
            int frame = frames[frameIdx];
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
                                sum += static_cast<PixelRGBA>(img.at(px, py, frame, layer)) * 
                                    kernel(ky + offset, kx + offset);
                            }
                        std::size_t pixelIdx =  ((frameIdx*layers.size() + layerIdx)*height + y)*width + x;
                        pixelBuffer[pixelIdx] = sum;
                        if (!filterAlpha)
                            pixelBuffer[pixelIdx].a = img.at(x, y, frame, layer).a;
                            
                    }
            }
        }

        for(int frameIdx = 0; frameIdx < frames.size(); ++frameIdx)
        {
            int frame = frames[frameIdx];
            for(int layerIdx = 0; layerIdx < layerIndices.size(); ++layerIdx)
            {
                int layer = layerIndices[layerIdx];
                for(int y = 0; y < height; ++y)
                    for(int x = 0; x < width; ++x)
                        {
                            std::size_t pixelIdx = ((frameIdx*layers.size() + layerIdx)*height + y)*width + x;
                            img.at(x, y, frame, layer) = pixelBuffer[pixelIdx];
                        }
            }
        }

    }

    REGISTER_FILTER(ConvolutionFilter)

} // namespace dmxdenoiser


