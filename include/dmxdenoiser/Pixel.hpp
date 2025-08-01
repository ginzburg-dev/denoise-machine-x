// Pixel.hpp
#pragma once

#include <dmxdenoiser/util/FloatsEqual.hpp>

namespace dmxdenoiser
{
    struct PixelRGBAView;
    
    struct PixelRGBA
    {
        float r;
        float g;
        float b;
        float a;

        operator PixelRGBAView();

        constexpr PixelRGBA& operator=(const PixelRGBA& p)
        {
            r = p.r;
            g = p.g;
            b = p.b;
            a = p.a;
            return *this;
        }

        constexpr PixelRGBA& operator+=(const PixelRGBA& p)
        {
            r += p.r;
            g += p.g;
            b += p.b;
            a += p.a;
            return *this;
        }

        constexpr PixelRGBA& operator-=(const PixelRGBA& p)
        {
            r -= p.r;
            g -= p.g;
            b -= p.b;
            a -= p.a;
            return *this;
        }

        constexpr PixelRGBA& operator*(float value)
        {
            r *= value;
            g *= value;
            b *= value;
            a *= value;
            return *this;
        }

        friend constexpr bool operator==(const PixelRGBA& p1, const PixelRGBA& p2) 
        { 
            return  floatsEqual(p1.r, p2.r) &&
                    floatsEqual(p1.g, p2.g) &&
                    floatsEqual(p1.b, p2.b) &&
                    floatsEqual(p1.a, p2.a);
        }

        friend constexpr bool operator!=(const PixelRGBA& p1, const PixelRGBA& p2) 
        { 
            return  !(p1 == p2);
        }
    };

    struct PixelRGBAView
    {
        float& r;
        float& g;
        float& b;
        float& a;

        [[nodiscard]] operator PixelRGBA() const{
            return {r, g, b, a};
        }
        
        constexpr PixelRGBAView& operator=(const PixelRGBAView& pv)
        {
            r = pv.r;
            g = pv.g;
            b = pv.b;
            a = pv.a;
            return *this;
        }

        constexpr PixelRGBAView& operator=(const PixelRGBA& p)
        {
            r = p.r;
            g = p.g;
            b = p.b;
            a = p.a;
            return *this;
        }

        friend constexpr bool operator==(const PixelRGBAView& pv1, const PixelRGBAView& pv2) 
        { 
            return  floatsEqual(pv1.r, pv2.r) &&
                    floatsEqual(pv1.g, pv2.g) &&
                    floatsEqual(pv1.b, pv2.b) &&
                    floatsEqual(pv1.a, pv2.a);
        }

        friend constexpr bool operator!=(const PixelRGBAView& pv1, const PixelRGBAView& pv2) 
        { 
            return  !(pv1 == pv2);
        }
    };

    inline PixelRGBA::operator PixelRGBAView()
    {
        return PixelRGBAView{r, g, b, a};
    }

} // namespace dmxdenoiser
