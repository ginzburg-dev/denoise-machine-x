// Config.hpp
#pragma once

namespace dmxdenoiser {
    inline constexpr int DEFAULT_NUM_CHANNELS = 4; // RGBA global internal fixed size for all images
} // namespace dmxdenoiser

#ifndef NDEBUG
    #define DMX_DEBUG_BUILD 1
#else
    #define DMX_DEBUG_BUILD 0
#endif


#if !defined(DMX_ENABLE_CUDA)
    #define DMX_ENABLE_CUDA 0
#endif

#if defined(__CUDAACC___)
    #define DMX_CPU_GPU __host__ __device__
    #if defined(_MSC_VER)
        #define DMX_INLINE __forceinline
    #else
        #define DMX_INLINE inline __attribute__((always_inline))
    #endif
#else
    #define DMX_CPU_GPU
    #if defined(_MSC_VER)
        #define DMX_INLINE __forceinline
    #else
        #define DMX_INLINE inline __attribute__((always_inline))
    #endif
#endif
