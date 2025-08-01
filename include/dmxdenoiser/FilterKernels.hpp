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
        /// @details Example (for kernel=3):
        ///     1/9 * [ 1 1 1 ]
        ///           [ 1 1 1 ]
        ///           [ 1 1 1 ]
        inline Kernel2D getBoxKernel(std::size_t kernel) 
        {
            if (kernel % 2 == 0)
                throw std::invalid_argument("Kernel size must be odd.");

            std::size_t size = kernel * kernel;
            float value = 1.0f / static_cast<float>(size);

            std::vector<float> boxKernel(size, value);

            return Kernel2D{boxKernel};
        }

        /// @brief Returns the standard 3x3 Sobel kernel for horizontal edge detection.
        inline Kernel2D getSobelKernelX()
        {
            return Kernel2D{std::vector<float>{
                -1, 0, 1, 
                -2, 0, 2, 
                -1, 0, 1
            }};
        }

        /// @brief Returns the standard 3x3 Sobel kernel for vertical edge detection.
        inline Kernel2D getSobelKernelY() 
        {
            return Kernel2D{std::vector<float>{
                -1, -2, -1, 
                0, 0, 0, 
                1, 2, 1
            }};
        }

    } // namespace FilterKernels

} // namespace dmxdenoiser
