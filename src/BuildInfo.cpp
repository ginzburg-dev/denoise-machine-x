#include <dmxdenoiser/BuildInfo.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/Version.hpp>

namespace dmxdenoiser 
{
    
    void logBuildBanner() {
        #ifdef DMX_DEBUG_BUILD
            constexpr const char* buildMode = "Release";
        #else
            constexpr const char* buildMode = "Debug";
        #endif

        DMX_LOG_INFO("BuildInfo", "\"", DMX_NAME, "\"(", DMX_SHORT_NAME, "), version: ",  DMX_VERSION, ", (", DMX_BUILD_DATE,"), ", buildMode );
    }

} // namespace dmxdenoiser 
