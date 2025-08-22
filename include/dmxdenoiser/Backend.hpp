// Backend.hpp
#pragma once

#include <dmxdenoiser/StringConversions.hpp>
#include <dmxdenoiser/ThreadPool.hpp>

#include <iostream>
#include <string>
#include <string_view>
#include <sstream>

namespace dmxdenoiser 
{

    enum class Backend
    {
        AUTO,
        CPU,
        GPU,
        METAL,
        // Add more as needed
    };

    struct BackendResource
    {
        // CPU
        ThreadPool* threadPool = nullptr;

        // CUDA
        int cudaDevice = 0;
        void* cudaStream = nullptr;

        // METAL
        void* metalDevice = nullptr;
        void* metalCommandQueue = nullptr;

        std::string ToString(std::size_t indent=0) const;
    };

    inline std::string ToString(Backend backend, std::size_t indent=0)
    {
        std::ostringstream oss;
        std::string sIndent = "";
            for (size_t i = 0; i < indent; ++i)
                sIndent += " ";
        switch(backend)
        {
            case Backend::AUTO: return sIndent + "Auto";
            case Backend::CPU: return sIndent + "Cpu";
            case Backend::GPU: return sIndent + "Gpu";
            case Backend::METAL: return sIndent + "Metal";
            default: return "Unknown backend";
        }
    }

    inline std::string BackendResource::ToString(std::size_t indent) const
    {
        std::ostringstream oss;
        std::string sIndent = "";
            for (size_t i = 0; i < indent; ++i)
                sIndent += " ";

        oss << sIndent << "BackendResource:\n";
        oss << sIndent << "     threadPool: " << static_cast<const void*>(threadPool) << '\n';
        oss << sIndent << "     cudaDevice: " << cudaDevice << '\n';
        oss << sIndent << "     metalDevice: " << static_cast<const void*>(metalDevice) << '\n';
        oss << sIndent << "     metalCommandQueue: " << static_cast<const void*>(metalCommandQueue);
        return oss.str();
    }

    inline std::ostream& operator<<(std::ostream& out, const BackendResource& backendResource) 
    {
        out << backendResource.ToString();
        return out;
    }

    /// Helper to parse "cpu"/"cuda"/"metal" etc.
    inline Backend parseBackend(const std::string& backend)
    {
        if (toLower(backend) == "auto") return Backend::AUTO;
        if (toLower(backend) == "cpu") return Backend::CPU;
        if (toLower(backend) == "gpu") return Backend::GPU;
        if (toLower(backend) == "metal") return Backend::METAL;
        return Backend::CPU;
    }

} // namespace dmxdenoiser
