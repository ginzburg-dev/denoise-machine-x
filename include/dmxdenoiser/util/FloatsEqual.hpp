// FloatsEqual.hpp
#pragma once

#include <cmath>
#include <limits>

namespace dmxdenoiser
{

    constexpr inline bool floatsEqual(float a, float b, float epsilon = std::numeric_limits<float>::epsilon())
    {
        return std::fabs(a - b) < epsilon;
    }

} // namespace dmxdenoiser
