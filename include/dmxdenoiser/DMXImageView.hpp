// DMXImageView
#pragma once

#include <dmxdenoiser/Config.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/DMXError.hpp>
#include <dmxdenoiser/Pixel.hpp>

namespace dmxdenoiser 
{

    struct DMXImageView
    {
        float* data;
        int width{};
        int height{};
        int numLayers{};
        int numFrames{};
        int numChannels = DEFAULT_NUM_CHANNELS;

        DMX_CPU_GPU
        bool inBounds(int x, int y, int frame, int layer) const {
            return  (x >= 0 && x < width ) &&
                    (y >= 0 && y < height ) &&
                    (frame >= 0 && frame < numFrames ) &&
                    (layer >= 0 && layer < numLayers );
        }

        DMX_CPU_GPU 
        float* at(int x, int y, int frame, int layer) {
            int index = getIndex(x, y, frame, layer);
            return &data[index];
        }

        DMX_CPU_GPU 
        const float* at(int x, int y, int frame, int layer) const {
            return const_cast<DMXImageView*>(this)->at(x, y, frame, layer);
        }

    private:
        DMX_CPU_GPU int getIndex(int x, int y, int frame, int layer) const {
            return (((frame * numLayers + layer) * height + y) * width + x) * numChannels;
        }

    };

} // namespace dmxdenoiser 
