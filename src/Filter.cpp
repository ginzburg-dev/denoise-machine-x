#include <dmxdenoiser/Backend.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/Kernel2D.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>
#include <dmxdenoiser/Pixel.hpp>
#include <dmxdenoiser/StringConversions.hpp>

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

namespace dmxdenoiser
{

    void Filter::convolve2D(
            const DMXImage& input,
            DMXImage& output,
            const Kernel2D& kernel,
            std::vector<int> frames,
            std::vector<std::string> layers,
            bool filterAlpha)
    {
        output = input;
        int width = input.width();
        int height = input.height();

        int ksize = kernel.size();
        int offset = ksize/2;
        
        std::vector<int> framesIndices;

        // If no specific frames were set, process all frames by default.
        if (frames.empty())
        {
            for (int i = 0; i < input.numFrames(); ++i)
                framesIndices.push_back(i);
        } else {
            framesIndices = frames;
        }

        std::vector<int> layerIndices;

        // If no specific layers were set, process by default.   
        if (layers.empty()) {
            layerIndices = input.getFilteringLayersIndices();
        } else {
            for (const auto& layer : layers)
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
                for(int y = 0; y < height; ++y)
                    for(int x = 0; x < width; ++x)
                    {
                        PixelRGBA sum = {0.0f, 0.0f, 0.0f, 0.0f};
                        for(int ky = -offset; ky <= offset; ++ky)
                            for(int kx = -offset; kx <= offset; ++kx)
                            {
                                int px = std::clamp(x + kx, 0, width - 1);
                                int py = std::clamp(y + ky, 0, height - 1);
                                sum += kernel(ky + offset, kx + offset) * input.get(px, py, frame, layer);
                            }
                        if (!filterAlpha)
                            sum.a = input.get(x, y, frame, layer).a;
                        output.at(x, y, frame, layer) = sum;
                    }
            }
        }
    }

} // namespace dmxdenoiser
