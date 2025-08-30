#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/DMXImageView.hpp>
#include <dmxdenoiser/Kernel2D.hpp>
#include <dmxdenoiser/Pixel.hpp>
#include <dmxdenoiser/utils/NumericUtils.hpp>

#include <cuda_runtime.h>

namespace dmxdenoiser
{

    __global__ void convolve2D_CUDA_kernel(DMXImageView in, DMXImageView out, int* frames, int framesSize, int* layers, int layersSize,
                                    float* kernel, int kernelSize, float strength, bool filterAlpha) 
    {
        int x = blockIdx.x * blockDim.x + threadIdx.x;
        int y = blockIdx.y * blockDim.y + threadIdx.y;
        int s = blockIdx.z;

        int frameIdx = layers[s / layersSize];
        int layerIdx = layers[s % layersSize];
        int frame = frames[frameIdx];
        int layer = layers[layerIdx];
        int offset = kernelSize/2;

        PixelRGBA orig = in.get(x, y, frame, layer);
        PixelRGBA sum = {0.0f, 0.0f, 0.0f, 0.0f};
        for(int ky = -offset; ky <= offset; ++ky)
            for(int kx = -offset; kx <= offset; ++kx)
            {
                int px = clampf(x + kx, 0, in.width - 1);
                int py = clampf(y + ky, 0, in.height - 1);
                sum += kernel[(ky + offset)*kernelSize + (kx + offset)] * in.get(px, py, frame, layer);
            }
        sum = blendPixels(orig, sum, strength, filterAlpha);
        out.at(x, y, frame, layer) = sum;
    }

    void convolve2D_CUDA(const DMXImage& in_, DMXImage& out_, std::vector<int> frames_, std::vector<int> layers_,
                                    const Kernel2D& kernel_, float strength, bool filterAlpha)
    {
        int framesSize = frames_.size();
        int* frames = new int[framesSize];
        int layersSize = layers_.size();
        int* layers = new int[layersSize];
        

        convolve2D_CUDA_kernel<<<>>>
    }

} // namespace dmxdenoiser
