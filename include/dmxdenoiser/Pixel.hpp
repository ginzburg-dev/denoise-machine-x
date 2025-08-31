// Pixel.hpp
#pragma once

#include <dmxdenoiser/utils/NumericUtils.hpp>
#include <dmxdenoiser/Config.hpp>

namespace dmxdenoiser
{
    struct PixelRGBAView;
    
    struct PixelRGBA
    {
        float r;
        float g;
        float b;
        float a;

        DMX_CPU_GPU DMX_INLINE explicit operator PixelRGBAView(); // 'explicit' to prevent implicit conversions (PixelRGBA <-> PixelRGBAView)

        DMX_CPU_GPU DMX_INLINE PixelRGBA& operator=(const PixelRGBA&) = default;
        DMX_CPU_GPU DMX_INLINE PixelRGBA& operator=(const PixelRGBAView& pv);
        DMX_CPU_GPU DMX_INLINE PixelRGBA& operator+=(const PixelRGBA& p) { r += p.r; g += p.g; b += p.b; a += p.a; return *this; }
        DMX_CPU_GPU DMX_INLINE PixelRGBA& operator+=(float value) { r += value; g += value; b += value; a += value; return *this; }
        DMX_CPU_GPU DMX_INLINE PixelRGBA& operator-=(const PixelRGBA& p) { r -= p.r; g -= p.g; b -= p.b; a -= p.a; return *this; }
        DMX_CPU_GPU DMX_INLINE PixelRGBA& operator-=(float value) { r -= value; g -= value; b -= value; a -= value; return *this; }
        DMX_CPU_GPU DMX_INLINE PixelRGBA& operator*=(const PixelRGBA& p) { r *= p.r; g *= p.g; b *= p.b; a *= p.a; return *this; }
        DMX_CPU_GPU DMX_INLINE PixelRGBA& operator*=(float value) { r *= value; g *= value; b *= value; a *= value; return *this; }
        DMX_CPU_GPU DMX_INLINE PixelRGBA& operator/=(const PixelRGBA& p) { r /= p.r; g /= p.g; b /= p.b; a /= p.a; return *this; }
        DMX_CPU_GPU DMX_INLINE PixelRGBA& operator/=(float value) { r /= value; g /= value; b /= value; a /= value; return *this; }
        DMX_CPU_GPU DMX_INLINE PixelRGBA operator-() const { return PixelRGBA{ -r, -g, -b, -a}; }

        DMX_CPU_GPU DMX_INLINE friend PixelRGBA operator+(const PixelRGBA& p1, const PixelRGBA& p2) {  
            return PixelRGBA{p1.r + p2.r, p1.g + p2.g, p1.b + p2.b, p1.a + p2.a};
        }
        DMX_CPU_GPU DMX_INLINE friend PixelRGBA operator+(const PixelRGBA& p, float value) {
            return PixelRGBA{ p.r + value, p.g + value, p.b + value, p.a + value };
        }
        DMX_CPU_GPU DMX_INLINE friend PixelRGBA operator+(float value, const PixelRGBA& p) { return p + value; }

        DMX_CPU_GPU DMX_INLINE friend PixelRGBA operator-(const PixelRGBA& p1, const PixelRGBA& p2) {  
            return PixelRGBA{p1.r - p2.r, p1.g - p2.g, p1.b - p2.b, p1.a - p2.a};
        }
        DMX_CPU_GPU DMX_INLINE friend PixelRGBA operator-(const PixelRGBA& p, float value) {
            return PixelRGBA{ p.r - value, p.g - value, p.b - value, p.a - value };
        }
        DMX_CPU_GPU DMX_INLINE friend PixelRGBA operator-(float value, const PixelRGBA& p) { return p - value; }

        DMX_CPU_GPU DMX_INLINE friend PixelRGBA operator*(const PixelRGBA& p1, const PixelRGBA& p2) {  
            return PixelRGBA{p1.r * p2.r, p1.g * p2.g, p1.b * p2.b, p1.a * p2.a};
        }
        DMX_CPU_GPU DMX_INLINE friend PixelRGBA operator*(const PixelRGBA& p, float value) {
            return PixelRGBA{ p.r * value, p.g * value, p.b * value, p.a * value };
        }
        DMX_CPU_GPU DMX_INLINE friend PixelRGBA operator*(float value, const PixelRGBA& p) { return p * value; }

        DMX_CPU_GPU DMX_INLINE friend PixelRGBA operator/(const PixelRGBA& p1, const PixelRGBA& p2) {  
            return PixelRGBA{p1.r / p2.r, p1.g / p2.g, p1.b / p2.b, p1.a / p2.a};
        }
        DMX_CPU_GPU DMX_INLINE friend PixelRGBA operator/(const PixelRGBA& p, float value) {
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
        [[nodiscard]] DMX_CPU_GPU DMX_INLINE explicit operator PixelRGBA() const{
            return {r, g, b, a};
        }

        DMX_CPU_GPU DMX_INLINE PixelRGBAView(float& r_, float& g_, float& b_, float& a_)
            : r{r_}, g{g_}, b{b_}, a{a_} 
        {}
        DMX_CPU_GPU DMX_INLINE PixelRGBAView(PixelRGBA& p)
            : r{p.r}, g{p.g}, b{p.b}, a{p.a} 
        {}
        DMX_CPU_GPU DMX_INLINE PixelRGBAView(const PixelRGBAView&) = delete;
        DMX_CPU_GPU DMX_INLINE PixelRGBAView(PixelRGBAView&&) = delete;
        DMX_CPU_GPU DMX_INLINE PixelRGBAView& operator=(PixelRGBAView&&) = delete;
        DMX_CPU_GPU DMX_INLINE PixelRGBAView& operator=(const PixelRGBAView& pv) { r = pv.r; g = pv.g; b = pv.b; a = pv.a; return *this; }
        DMX_CPU_GPU DMX_INLINE PixelRGBAView& operator=(const PixelRGBA& p) { r = p.r; g = p.g; b = p.b; a = p.a; return *this; }
    };

    DMX_CPU_GPU DMX_INLINE PixelRGBA::operator PixelRGBAView() { return PixelRGBAView{r, g, b, a}; }
    DMX_CPU_GPU DMX_INLINE PixelRGBA& PixelRGBA::operator=(const PixelRGBAView& pv) { r = pv.r; g = pv.g; b = pv.b; a = pv.a; return *this; };

    DMX_CPU_GPU DMX_INLINE bool operator==(const PixelRGBA& p1, const PixelRGBA& p2) { 
        return  floatsEqual(p1.r, p2.r) &&
                floatsEqual(p1.g, p2.g) &&
                floatsEqual(p1.b, p2.b) &&
                floatsEqual(p1.a, p2.a);
    }
    DMX_CPU_GPU DMX_INLINE bool operator==(const PixelRGBAView& pv1, const PixelRGBAView& pv2) { 
        return  floatsEqual(pv1.r, pv2.r) &&
                floatsEqual(pv1.g, pv2.g) &&
                floatsEqual(pv1.b, pv2.b) &&
                floatsEqual(pv1.a, pv2.a);
    }

    DMX_CPU_GPU DMX_INLINE bool operator==(const PixelRGBA& p, const PixelRGBAView& pv) { 
        return  floatsEqual(p.r, pv.r) &&
                floatsEqual(p.g, pv.g) &&
                floatsEqual(p.b, pv.b) &&
                floatsEqual(p.a, pv.a);
    }
    DMX_CPU_GPU DMX_INLINE bool operator==(const PixelRGBAView& pv, const PixelRGBA& p) { return  p == pv; }

    DMX_CPU_GPU DMX_INLINE bool operator!=(const PixelRGBA& p1, const PixelRGBA& p2) { return  !(p1 == p2); }
    DMX_CPU_GPU DMX_INLINE bool operator!=(const PixelRGBAView& pv1, const PixelRGBAView& pv2) { return  !(pv1 == pv2); }

    DMX_CPU_GPU DMX_INLINE bool operator!=(const PixelRGBA& p, const PixelRGBAView& pv) { return  !(p == pv); }
    DMX_CPU_GPU DMX_INLINE bool operator!=(const PixelRGBAView& pv, const PixelRGBA& p) { return  p != pv; }

    DMX_CPU_GPU DMX_INLINE PixelRGBA blendPixels(const PixelRGBA& a, const PixelRGBA& b, float t, bool alphaBlend=false) noexcept {
        PixelRGBA p{ 
            floatsBlend(a.r, b.r, t), 
            floatsBlend(a.g, b.g, t),
            floatsBlend(a.b, b.b, t),
            alphaBlend ? floatsBlend(a.a, b.a, t) : a.a
        };
        return p;
    }

} // namespace dmxdenoiser
