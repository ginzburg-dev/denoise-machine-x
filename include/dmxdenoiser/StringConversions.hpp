#pragma once

#include <string>
#include <sstream>
#include <iomanip>

namespace dmxdenoiser{
    namespace utils{

        inline std::string trimTrailingZeros(const std::string& str)
        {
            auto dec = str.find(".");
            if (dec == std::string::npos) return str;
            auto last = str.find_last_not_of('0');
            if (last == dec) last--; // Remove decimal if it's the last character
            return str.substr(0, last + 1);
        }

        /// @brief Converts a float to string with controlled decimal places (e.g., "1.25" instead of "1.250000").
        /// @details Ensures floating-point output is clean and consistent for display or logging.
        /// @param value The float or double value to convert.
        /// @param precision  Number of decimal places to use (default: 8).
        /// @return std::string Formatted string representation of the float.
        template<typename T>
        inline std::string floatToString(T value, int precision = 8)
        {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(precision) << value;
            return trimTrailingZeros(oss.str());
        }

    }
}
