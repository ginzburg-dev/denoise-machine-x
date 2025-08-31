// Config.hpp
#pragma once

#ifndef NDEBUG
    #define DMX_DEBUG_BUILD 1
#else
    #define DMX_DEBUG_BUILD 0
#endif


#if !defined(DMX_ENABLE_CUDA)
    #define DMX_ENABLE_CUDA 0
#endif

#if defined(__CUDAACC__)
    #define DMX_CPU_GPU __host__ __device__
#else
    #define DMX_CPU_GPU
#endif
