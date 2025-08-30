#include <dmxdenoiser/DMXImageView.hpp>
#include <dmxdenoiser/Pixel.hpp>

#include <cuda_runtime.h>

__global__ void convolve2D_CUDA(DMXImageView in, DMXImageView out, int* frames, int framesSize, int* layers, int layersSize,
                                float* kernel, int kernelSize) {
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
            int px = std::clamp(to_int(x) + kx, 0, width - 1);
            int py = std::clamp(to_int(y) + ky, 0, height - 1);
            sum += m_kernel(ky + offset, kx + offset) * input.get(px, py, frame, layer);
        }
    sum = blendPixels(orig, sum, m_strength, m_filterAlpha);
    output.at(to_int(x), to_int(y), frame, layer) = sum;
     
}
