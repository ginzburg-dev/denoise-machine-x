#include <dmxdenoiser/BuildInfo.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/Version.hpp>

namespace dmxdenoiser 
{
    
    void logBuildBanner() {

        #if DMX_DEBUG_BUILD
            #if defined(_MSC_VER)
                #pragma message("Build mode: DEBUG (NDEBUG not defined)")
            #else
                #warning "Build mode: DEBUG (NDEBUG not defined)"
            #endif
            constexpr const char* buildMode = "Debug";
        #else
            #if defined(_MSC_VER)
                #pragma message("Build mode: RELEASE (NDEBUG defined)")
            #else
                #warning "Build mode: RELEASE (NDEBUG defined)"
            #endif
            constexpr const char* buildMode = "Release";
        #endif

        DMX_LOG_INFO("BuildInfo", "\"", DMX_NAME, "\"(", DMX_SHORT_NAME, "), version: ",  DMX_VERSION, ", (", DMX_BUILD_DATE,"), ", buildMode );
    }

} // namespace dmxdenoiser 
