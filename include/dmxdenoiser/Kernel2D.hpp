// Kernel2D.hpp
#pragma once

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

namespace dmxdenoiser 
{

    /// @brief 1D array of floats that represents a 2D filter kernel (matrix).
    /// @note The kernel array must be a non-empty, odd-sized perfect square:
    /// 1x1, 3x3, 5x5, 7x7, etc. (e.g., 1, 9, 25, 49 elements)
    struct Kernel2D
    {
        std::vector<float> m_data{};
        std::string m_name{};
        std::size_t m_size{};

        Kernel2D() = default;
        Kernel2D(const Kernel2D&) = default;
        Kernel2D(const std::string& name, std::vector<float> values)
        {
            set(name, std::move(values));
        }

        void set(const std::string& name, std::vector<float> values)
        {
            m_data = std::move(values);
            m_name = name;

            double sq = std::sqrt(m_data.size());
            if (sq != std::floor(sq))
                throw std::invalid_argument("Kernel2D: data must be a perfect square.");

            if (m_data.empty())
                throw std::invalid_argument("Kernel2D: data must be a non-empty.");

            if (static_cast<std::size_t>(sq) % 2 == 0)
                throw std::invalid_argument("Kernel2D: kernel size must be odd.");

            m_size = static_cast<std::size_t>(sq);
        }
        void set(const Kernel2D& k) { set(k.name(), k.m_data); }

        std::size_t size() const { return m_size; }
        const std::string& name() const { return m_name; }

        float operator()(std::size_t y, std::size_t x) const 
        {
            if ( y >= m_size || x >= m_size )
                throw std::invalid_argument("Kernel2D: index out of bounds.");
            return m_data[y*m_size + x];
        }

        void clear() {
            m_data.clear();
            m_name = "";
            m_size = 0;
        }

        std::string ToString() const
        {
            std::ostringstream oss;
            oss << "Kernel2D: " << name() << " " << m_size << " x " << m_size << "\n";
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

    inline std::ostream& operator<<(std::ostream& out, const Kernel2D& kernel)
    {
        out << kernel.ToString();
        return out;
    }

} // namespace dmxdenoiser 
