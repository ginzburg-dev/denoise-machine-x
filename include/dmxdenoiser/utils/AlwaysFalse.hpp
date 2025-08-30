// AlwaysFalse.hpp
#pragma once

namespace dmxdenoiser
{

    template<class> 
    struct always_false{
        static constexpr bool value = false;
    };

} // namespace dmxdenoiser
