#pragma once

#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility>

namespace dmxdenoiser{

        inline std::string toLower(const std::string& s)
        {
            std::string result = s;
            std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c){ return std::tolower(c); });
            return result;
        }

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

        // has_ToString_external<T>: true if ToString(const T&) is callable and returns std::string
        template<class T, class = void>
        struct has_ToString_external { static constexpr int value = false; };
        template<typename T>
        struct has_ToString_external<T, 
            std::void_t<decltype(ToString(std::declval<const T&>()))>> { static constexpr int value = true; };
        template<typename T>
        inline constexpr bool has_ToString_external_v = has_ToString_external<T>::value;

        // has_ToString_method<T>: true if T has ToString() method
        template<class T, class = void>
        struct has_ToString_method { static constexpr int value = false; };
        template<typename T>
        struct has_ToString_method<T, 
            std::void_t<decltype(std::declval<T>().ToString())>> { static constexpr int value = true; };
        template<typename T>
        inline constexpr bool has_ToString_method_v = has_ToString_method<T>::value;

        // joinVector<T>: serializes a vector<T> to string, e.g. v = {s1, s2, s3} → s = "[s1, s2, s3]"
        template<typename T>
        std::string joinVector(const std::vector<T>& v,
                            const std::string separator = ", ",
                            const std::string prefix = "[",
                            const std::string postfix = "]",
                            const std::string emptyStr = "empty")
        {
            if (v.empty())
                return prefix + emptyStr + postfix;

            std::ostringstream oss;
            oss << std::boolalpha << prefix;
            for (int i = 0; i < v.size(); ++i)
            {
                if constexpr (has_ToString_method_v<T>) {
                    oss << v[i].ToString();
                } else if constexpr (has_ToString_external_v<T>) {
                    oss << ToString(v[i]);
                } else  if constexpr (std::is_same_v<T, char>) { // char
                    oss << std::to_string(1, v[i]);
                } else  if constexpr (std::is_same_v<T, bool>) { // bool
                    oss << (v[i] ? "true" : "false");
                } else  if constexpr (std::is_convertible_v<T, std::string_view>) { // std::string_view
                    oss << std::string(std::string_view(v[i]));
                }  else if constexpr (std::is_floating_point_v<T>) { // float/ints
                    oss << floatToString(v[i]);
                } else  if constexpr (std::is_arithmetic_v<T>) { // ints/etc
                    oss << std::to_string(v[i]);
                } else {
                    static_assert([]{return false;}(),
                        "joinVector: T is not printable. Provide ToString(T) or T::ToString() or string-like.");
                }
                if (i != v.size()-1)
                    oss << separator;
            }
            oss << postfix;
            return oss.str();
        }
        
} // namespace dmxdenoiser
