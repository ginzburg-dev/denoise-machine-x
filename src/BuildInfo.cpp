#include <dmxdenoiser/BuildInfo.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/Version.hpp>

namespace dmxdenoiser 
{
    
    void logBuildBanner() {

        #if DMX_DEBUG_BUILD
            constexpr const char* kBuildMode = "Debug";
        #else
            constexpr const char* kBuildMode = "Release";
        #endif

        DMX_LOG_INFO(
            "BuildInfo", 
            "\"", kProductName, "\" "
            "(", kProductShortName, "), ",
            "version: ", kBuildVersion, ", ",
            "(", kBuildDate, "), ",
            "build mode: ", kBuildMode
        );
    }

} // namespace dmxdenoiser 
