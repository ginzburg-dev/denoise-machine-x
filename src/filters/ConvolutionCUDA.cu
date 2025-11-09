#include <dmxdenoiser/filters/ConvolutionCUDA.hpp>
#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/DMXImageView.hpp>
#include <dmxdenoiser/Kernel2D.hpp>
#include <dmxdenoiser/utils/NumericUtils.hpp>
#include <dmxdenoiser/Pixel.hpp>

#include <cstddef>

#include <cuda_runtime.h>

#define CUDA_CHECK(x) do { cudaError_t _e = (x); if (_e != cudaSuccess) \
    throw std::runtime_error(std::string("CUDA error: ")+cudaGetErrorString(_e)); } while(0)

namespace dmxdenoiser
{

    __global__ void convolve2D_CUDA_kernel(DMXImageView in, DMXImageView out, int* frames, int framesSize, int* layers, int layersSize,
                                    float* kernel, int kernelSize, float strength, bool filterAlpha) 
    {
        int x = blockIdx.x * blockDim.x + threadIdx.x;
        int y = blockIdx.y * blockDim.y + threadIdx.y;
        int s = blockIdx.z;

        if (x >= in.width || y >= in.height) return;

        const int total = framesSize * layersSize;
        if (s >= total) return;

        int frameIdx = s / layersSize;
        int layerIdx = s % layersSize;
        int frame = frames[frameIdx];
        int layer = layers[layerIdx];
        int offset = kernelSize >> 1;

        float* orig = in.at(x, y, frame, layer);
        float sum_r = 0.0f; float sum_g = 0.0f; float sum_b = 0.0f; float sum_a = 0.0f; 
        for(int ky = -offset; ky <= offset; ++ky) 
        {
            int py = clampi(y + ky, 0, in.height - 1);
            int krow = (ky + offset)*kernelSize;
            for(int kx = -offset; kx <= offset; ++kx)
            {
                int px = clampi(x + kx, 0, in.width - 1);
                float w = kernel[krow + (kx + offset)];
                float* p = in.at(px, py, frame, layer);
                sum_r += w * p[0];
                sum_g += w * p[1];
                sum_b += w * p[2];
                sum_a += w * p[3];
            }
        }
        float out_r = floatsBlend(orig[0], sum_r, strength);
        float out_g = floatsBlend(orig[1], sum_g, strength);
        float out_b = floatsBlend(orig[2], sum_b, strength);
        float out_a = filterAlpha ? floatsBlend(orig[3], sum_a, strength) : orig[3];

        float* dist = out.at(x, y, frame, layer);
        dist[0] = out_r; dist[1] = out_g; dist[2] = out_b; dist[3] = out_a;
    }

    void convolve2D_CUDA(const DMXImage& in_, DMXImage& out_, const std::vector<int>& frames_, const std::vector<int>& layers_,
                                    const Kernel2D& kernel_, float strength, bool filterAlpha)
    {
        DMXImage out_tmp = in_;

        int framesSize = frames_.size();
        int layersSize = layers_.size();
        int kernelSize = kernel_.size();
        std::size_t N = in_.data().size();

        int* d_frames = nullptr;
        int* d_layers = nullptr;
        float* d_kernel = nullptr;
        float* d_in = nullptr;
        float* d_out = nullptr;
        
        DMXImageView in;
        in.width = in_.width();
        in.height = in_.height();
        in.numLayers = in_.numLayers();
        in.numFrames = in_.numFrames();
        in.numChannels = in_.numChannels();
        DMXImageView out = in;

        CUDA_CHECK(cudaMalloc(&d_frames, framesSize*sizeof(int)));
        CUDA_CHECK(cudaMalloc(&d_layers, layersSize*sizeof(int)));
        CUDA_CHECK(cudaMalloc(&d_kernel, kernelSize*kernelSize*sizeof(float)));
        CUDA_CHECK(cudaMalloc(&d_in, N*sizeof(float)));
        CUDA_CHECK(cudaMalloc(&d_out, N*sizeof(float)));

        CUDA_CHECK(cudaMemcpy(d_frames, frames_.data(), framesSize*sizeof(int), cudaMemcpyHostToDevice));
        CUDA_CHECK(cudaMemcpy(d_layers, layers_.data(), layersSize*sizeof(int), cudaMemcpyHostToDevice));
        CUDA_CHECK(cudaMemcpy(d_kernel, kernel_.m_data.data(), kernelSize*kernelSize*sizeof(float), cudaMemcpyHostToDevice));
        CUDA_CHECK(cudaMemcpy(d_in, in_.data().data(), N*sizeof(float), cudaMemcpyHostToDevice));

        in.data = d_in;
        out.data = d_out;

        dim3 threads(16, 16, 1);
        dim3 blocks( 
            (in_.width() + threads.x - 1)/threads.x,
            (in_.height() + threads.y - 1)/threads.y,
            std::max(1, framesSize * layersSize)
        );
        
        convolve2D_CUDA_kernel<<<blocks, threads>>>(in, out, d_frames, framesSize, d_layers, 
            layersSize, d_kernel, kernelSize, strength, filterAlpha);
        CUDA_CHECK(cudaGetLastError());
        CUDA_CHECK(cudaDeviceSynchronize());
        
        CUDA_CHECK(cudaMemcpy(out_tmp.data().data(), out.data, N*sizeof(float), cudaMemcpyDeviceToHost));

        out_ = std::move(out_tmp);

        cudaFree(d_in);
        cudaFree(d_out);
        cudaFree(d_kernel);
        cudaFree(d_frames);
        cudaFree(d_layers);
    }

} // namespace dmxdenoiser
