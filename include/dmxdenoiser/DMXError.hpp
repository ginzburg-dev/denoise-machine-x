// DMXError.hpp
#pragma once

#include <dmxdenoiser/Config.hpp>

#include <stdio.h>

enum class DMXError
{
    dmxSuccess = 0,
    dmxOutOfBound = 1,
};

#if define(__CUDA_ARCH__)
    #define DMX_TRAP() asm("trap;")
#else
    #include <stdexcept>
    #define DMX_TRAP(MSG) std::runtime_error(MSG)
#endif
