// Pixel.hpp
#pragma once

#include <dmxdenoiser/util/NumericUtils.hpp>

namespace dmxdenoiser
{
    struct PixelRGBAView;
    
    struct PixelRGBA
    {
        float r;
        float g;
        float b;
        float a;

        explicit operator PixelRGBAView(); // 'explicit' to prevent implicit conversions (PixelRGBA <-> PixelRGBAView)

        PixelRGBA& operator=(const PixelRGBA&) = default;
        PixelRGBA& operator=(const PixelRGBAView& pv);
        PixelRGBA& operator+=(const PixelRGBA& p) { r += p.r; g += p.g; b += p.b; a += p.a; return *this; }
        PixelRGBA& operator+=(float value) { r += value; g += value; b += value; a += value; return *this; }
        PixelRGBA& operator-=(const PixelRGBA& p) { r -= p.r; g -= p.g; b -= p.b; a -= p.a; return *this; }
        PixelRGBA& operator-=(float value) { r -= value; g -= value; b -= value; a -= value; return *this; }
        PixelRGBA& operator*=(const PixelRGBA& p) { r *= p.r; g *= p.g; b *= p.b; a *= p.a; return *this; }
        PixelRGBA& operator*=(float value) { r *= value; g *= value; b *= value; a *= value; return *this; }
        PixelRGBA& operator/=(const PixelRGBA& p) { r /= p.r; g /= p.g; b /= p.b; a /= p.a; return *this; }
        PixelRGBA& operator/=(float value) { r /= value; g /= value; b /= value; a /= value; return *this; }
        PixelRGBA operator-() const { return PixelRGBA{ -r, -g, -b, -a}; }

        friend PixelRGBA operator+(const PixelRGBA& p1, const PixelRGBA& p2) {  
            return PixelRGBA{p1.r + p2.r, p1.g + p2.g, p1.b + p2.b, p1.a + p2.a};
        }
        friend PixelRGBA operator+(const PixelRGBA& p, float value) {
            return PixelRGBA{ p.r + value, p.g + value, p.b + value, p.a + value };
        }
        friend PixelRGBA operator+(float value, const PixelRGBA& p) { return p + value; }

        friend PixelRGBA operator-(const PixelRGBA& p1, const PixelRGBA& p2) {  
            return PixelRGBA{p1.r - p2.r, p1.g - p2.g, p1.b - p2.b, p1.a - p2.a};
        }
        friend PixelRGBA operator-(const PixelRGBA& p, float value) {
            return PixelRGBA{ p.r - value, p.g - value, p.b - value, p.a - value };
        }
        friend PixelRGBA operator-(float value, const PixelRGBA& p) { return p - value; }

        friend PixelRGBA operator*(const PixelRGBA& p1, const PixelRGBA& p2) {  
            return PixelRGBA{p1.r * p2.r, p1.g * p2.g, p1.b * p2.b, p1.a * p2.a};
        }
        friend PixelRGBA operator*(const PixelRGBA& p, float value) {
            return PixelRGBA{ p.r * value, p.g * value, p.b * value, p.a * value };
        }
        friend PixelRGBA operator*(float value, const PixelRGBA& p) { return p * value; }

        friend PixelRGBA operator/(const PixelRGBA& p1, const PixelRGBA& p2) {  
            return PixelRGBA{p1.r / p2.r, p1.g / p2.g, p1.b / p2.b, p1.a / p2.a};
        }
        friend PixelRGBA operator/(const PixelRGBA& p, float value) {
            return PixelRGBA{ p.r / value, p.g / value, p.b / value, p.a / value };
        }
        
    };

    struct PixelRGBAView
    {
        float& r;
        float& g;
        float& b;
        float& a;

        /// @brief Conversion operator to PixelRGBA.
        /// @note Marked [[nodiscard]] to warn if the result is not used.
        [[nodiscard]] explicit operator PixelRGBA() const{
            return {r, g, b, a};
        }

        PixelRGBAView(float& r_, float& g_, float& b_, float& a_)
            : r{r_}, g{g_}, b{b_}, a{a_} 
        {}
        PixelRGBAView(PixelRGBA& p)
            : r{p.r}, g{p.g}, b{p.b}, a{p.a} 
        {}
        PixelRGBAView(const PixelRGBAView&) = delete;
        PixelRGBAView(PixelRGBAView&&) = delete;
        PixelRGBAView& operator=(PixelRGBAView&&) = delete;
        PixelRGBAView& operator=(const PixelRGBAView& pv) { r = pv.r; g = pv.g; b = pv.b; a = pv.a; return *this; }
        PixelRGBAView& operator=(const PixelRGBA& p) { r = p.r; g = p.g; b = p.b; a = p.a; return *this; }
    };

    inline PixelRGBA::operator PixelRGBAView() { return PixelRGBAView{r, g, b, a}; }
    inline PixelRGBA& PixelRGBA::operator=(const PixelRGBAView& pv) { r = pv.r; g = pv.g; b = pv.b; a = pv.a; return *this; };

    inline bool operator==(const PixelRGBA& p1, const PixelRGBA& p2) { 
        return  floatsEqual(p1.r, p2.r) &&
                floatsEqual(p1.g, p2.g) &&
                floatsEqual(p1.b, p2.b) &&
                floatsEqual(p1.a, p2.a);
    }
    inline bool operator==(const PixelRGBAView& pv1, const PixelRGBAView& pv2) { 
        return  floatsEqual(pv1.r, pv2.r) &&
                floatsEqual(pv1.g, pv2.g) &&
                floatsEqual(pv1.b, pv2.b) &&
                floatsEqual(pv1.a, pv2.a);
    }

    inline bool operator==(const PixelRGBA& p, const PixelRGBAView& pv) { 
        return  floatsEqual(p.r, pv.r) &&
                floatsEqual(p.g, pv.g) &&
                floatsEqual(p.b, pv.b) &&
                floatsEqual(p.a, pv.a);
    }
    inline bool operator==(const PixelRGBAView& pv, const PixelRGBA& p) { return  p == pv; }

    inline bool operator!=(const PixelRGBA& p1, const PixelRGBA& p2) { return  !(p1 == p2); }
    inline bool operator!=(const PixelRGBAView& pv1, const PixelRGBAView& pv2) { return  !(pv1 == pv2); }

    inline bool operator!=(const PixelRGBA& p, const PixelRGBAView& pv) { return  !(p == pv); }
    inline bool operator!=(const PixelRGBAView& pv, const PixelRGBA& p) { return  p != pv; }

    inline PixelRGBA blendPixels(const PixelRGBA& a, const PixelRGBA& b, float t, bool alphaBlend=false) noexcept {
        PixelRGBA p{ 
            floatsBlend(a.r, b.r, t), 
            floatsBlend(a.g, b.g, t),
            floatsBlend(a.b, b.b, t),
            alphaBlend ? floatsBlend(a.a, b.a, t) : a.a
        };
        return p;
    }

} // namespace dmxdenoiser
