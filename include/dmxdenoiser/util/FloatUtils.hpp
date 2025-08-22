// FloatUtils.hpp
#pragma once

#include <algorithm> // for std::clamp()
#include <cmath>
#include <limits>

namespace dmxdenoiser
{

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
    /*
    constexpr inline bool floatsEqual(float a, float b, float epsilon = std::numeric_limits<float>::epsilon())
    {
        if (std::isnan(a) && std::isnan(b))
            return true; // Both are NaN
        if (std::isinf(a) && std::isinf(b))
            return true; // Both are +Inf or both are -Inf
        return std::fabs(a - b) < epsilon;
    }
    */

    // Blend two floats 
    constexpr inline float floatsBlend(float a, float b, float t) noexcept { 
        //t = std::clamp(t, 0.0f, 1.0f);
        t = t > 1.0f ? 1.0f : (t < 0.0f ? 0.0f : t);
        return a + (b - a) * t;
    } 

} // namespace dmxdenoiser
