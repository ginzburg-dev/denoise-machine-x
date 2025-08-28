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
