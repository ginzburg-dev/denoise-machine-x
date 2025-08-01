// FloatsEqual.hpp
#pragma once

#include <cmath>
#include <limits>

namespace dmxdenoiser
{

    constexpr inline bool floatsEqual(float a, float b, float epsilon = std::numeric_limits<float>::epsilon())
    {
        if (std::isnan(a) && std::isnan(b))
            return true; // Both are NaN
        if (std::isinf(a) && std::isinf(b))
            return true; // Both are +Inf or both are -Inf
        return std::fabs(a - b) < epsilon;
    }

} // namespace dmxdenoiser
