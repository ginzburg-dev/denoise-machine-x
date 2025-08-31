// FloatUtils.hpp
#pragma once

#include <dmxdenoiser/Config.hpp>

#include <algorithm> // for std::clamp()
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <limits>

namespace dmxdenoiser
{
    // static cast to int
    template<typename T> constexpr int to_int(T v) noexcept { return static_cast<int>(v); }
    // static cast to int64_t
    template<typename T> constexpr std::int64_t to_i64(T v) noexcept { return static_cast<std::int64_t>(v); }
    // static cast to size_t
    template<typename T> constexpr std::size_t to_size(T v) noexcept { return static_cast<std::size_t>(v); }

    constexpr bool is_nan_c(float x) noexcept { return x != x; } //Nan != Nan

    constexpr float abs_c(float x) noexcept { return x < float{0} ? -x : x; }

    constexpr bool is_inf_c(float x) noexcept {
        return  x == std::numeric_limits<float>::infinity() ||
                x == -std::numeric_limits<float>::infinity();
    }

    constexpr inline bool floatsEqual(float a, float b, float epsilon = std::numeric_limits<float>::epsilon()) noexcept
    {
        if (is_nan_c(a) && is_nan_c(b)) {
            return true;
        } else if (is_inf_c(a) && is_inf_c(b)) {
            return true;
        }
        return abs_c(a - b) < epsilon;
    }

    DMX_CPU_GPU
    constexpr inline float clampf(float x, float lo, float hi) {
        return (x > hi) ? hi : ((x < lo) ? lo : x);
    }

    // Blend two floats 
    DMX_CPU_GPU
    constexpr inline float floatsBlend(float a, float b, float t) noexcept { 
        t = clampf(t, 0.0f, 1.0f);
        return a + (b - a) * t;
    } 

} // namespace dmxdenoiser
