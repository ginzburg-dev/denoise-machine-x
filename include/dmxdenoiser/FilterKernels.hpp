// FilterKernels.hpp
#pragma once

#include <cmath>
#include <cstddef>
#include <sstream>
#include <vector>
#include <iomanip>

namespace dmxdenoiser 
{

    /// @brief 1D array of floats that represents a 2D filter kernel (matrix).
    struct Kernel2D
    {
        std::vector<float> m_data{};
        std::size_t m_size{};

        Kernel2D() = default;
        Kernel2D(std::vector<float> values)
        {
            set(std::move(values));
        }

        void set(std::vector<float> values)
        {
            m_data = std::move(values);

            double sq = std::sqrt(m_data.size());
            if (sq != std::floor(sq) || m_data.empty() )
                throw std::invalid_argument("Kernel2D: data must be a non-empty perfect square.");

            if (static_cast<std::size_t>(sq) % 2 == 0)
                throw std::invalid_argument("Kernel2D: kernel size must be odd.");

            m_size = static_cast<std::size_t>(sq);
        }

        std::size_t size() const { return m_size; }

        float operator()(std::size_t y, std::size_t x) const {
            if ( y >= m_size || x >= m_size )
                throw std::invalid_argument("Kernel2D: index out of bounds.");
            return m_data[y*m_size + x];
        }

        std::string ToString() const
        {
            std::ostringstream oss;
            oss << "Kernel2D: " << m_size << " x " << m_size << "\n";
            for (int y = 0; y < m_size; ++y)
            {
                oss << "    [";
                for (int x = 0; x < m_size; ++x)
                {
                    oss << std::fixed << std::setprecision(4) << (*this)(y, x);
                    if (x != m_size - 1)
                        oss << " ";
                }
                oss << "]\n";
            }
            return oss.str();
        }
    };

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

    } // namespace Kernel

} // namespace dmxdenoiser
