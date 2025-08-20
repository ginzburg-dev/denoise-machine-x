// Config.hpp
#pragma once

#ifndef NDEBUG
    #define DMX_DEBUG_BUILD 1
#else
    #define DMX_DEBUG_BUILD 0
#endif

#if DMX_DEBUG_BUILD
    #if defined(_MSC_VER)
        #pragma message("Build mode: DEBUG (NDEBUG not defined)")
    #else
        #warning "Build mode: DEBUG (NDEBUG not defined)"
    #endif
#else
    #if defined(_MSC_VER)
        #pragma message("Build mode: RELEASE (NDEBUG defined)")
    #else
        #warning "Build mode: RELEASE (NDEBUG defined)"
    #endif
#endif
