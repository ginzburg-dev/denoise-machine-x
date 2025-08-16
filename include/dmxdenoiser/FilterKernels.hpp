// FilterKernels.hpp
#pragma once

#include <dmxdenoiser/Kernel2D.hpp>

#include <cstddef>
#include <vector>

namespace dmxdenoiser 
{

    namespace FilterKernels
    {

        /// @brief Returns a box filter kernel of size kernel x kernel.
        /// Example (for kernel=3):
        ///     1/9 * [ 1 1 1 ]
        ///           [ 1 1 1 ]
        ///           [ 1 1 1 ]
        inline Kernel2D getBoxKernel(std::size_t kernelSize) 
        {
            if (kernelSize % 2 == 0)
                throw std::invalid_argument("Kernel size must be odd.");

            std::size_t size = kernelSize * kernelSize;
            float value = 1.0f / static_cast<float>(size);

            std::vector<float> boxKernel(size, value);

            return Kernel2D{"Box", boxKernel};
        }

        inline Kernel2D getGaussianKernel(std::size_t kernelSize, float sigma) 
        {
            if (kernelSize % 2 == 0)
                throw std::invalid_argument("Kernel size must be odd.");
            
            int mu = static_cast<int>(kernelSize/2);
            std::size_t size = kernelSize * kernelSize;
            std::vector<float> gaussianKernel(size);

            float sum = 0.0f;
            for(int y = 0; y < kernelSize; ++y)
                for (int x = 0; x < kernelSize; ++x)
                {
                    float dist = static_cast<float>((x-mu)*(x-mu) + (y-mu)*(y-mu));
                    float value = std::exp(-dist/(2*sigma*sigma));
                    gaussianKernel[y*kernelSize + x] = value;
                    sum += value;
                }

            // Normalize kernel
            for (auto& v : gaussianKernel)
                v /= sum;

            return Kernel2D{"Gaussian", gaussianKernel};
        }

        /// @brief Returns the standard 3x3 Sobel kernel for horizontal edge detection.
        inline Kernel2D getSobelKernelX()
        {
            return Kernel2D{"SobelX", std::vector<float>{
                -1, 0, 1, 
                -2, 0, 2, 
                -1, 0, 1
            }};
        }

        /// @brief Returns the standard 3x3 Sobel kernel for vertical edge detection.
        inline Kernel2D getSobelKernelY() 
        {
            return Kernel2D{"SobelX", std::vector<float>{
                -1, -2, -1, 
                0, 0, 0, 
                1, 2, 1
            }};
        }

    } // namespace FilterKernels

} // namespace dmxdenoiser
