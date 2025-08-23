#include <dmxdenoiser/BuildInfo.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/Version.hpp>

namespace dmxdenoiser 
{
    
    void logBuildBanner() {

        #if DMX_DEBUG_BUILD
            constexpr const char* buildMode = "Debug";
        #else
            constexpr const char* buildMode = "Release";
        #endif

        DMX_LOG_INFO("BuildInfo", "\"", DMX_NAME, "\" (", DMX_SHORT_NAME, "), version: ",  DMX_VERSION, ", (", DMX_BUILD_DATE,"), build mode: ", buildMode );
    }

} // namespace dmxdenoiser 
