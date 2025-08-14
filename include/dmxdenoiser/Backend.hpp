// Backend.hpp
#pragma once

#include <dmxdenoiser/StringConversions.hpp>

#include <iostream>
#include <string>
#include <string_view>
#include <sstream>

namespace dmxdenoiser 
{

    enum class Backend
    {
        CPU,
        CUDA,
        METAL,
        // Add more as needed
    };

    struct BackendResource
    {
        // CPU
        void* threadPool = nullptr;

        // CUDA
        int cudaDevice = 0;
        void* cudaStream = nullptr;

        // METAL
        void* metalDevice = nullptr;
        void* metalCommandQueue = nullptr;

        std::string ToString() const;
    };

    inline std::string BackendResource::ToString() const
    {
        std::ostringstream oss;
        oss << "BackendResource:\n";
        oss << "    threadPool: " << static_cast<const void*>(threadPool) << '\n';
        oss << "    cudaDevice: " << cudaDevice << '\n';
        oss << "    metalDevice: " << static_cast<const void*>(metalDevice) << '\n';
        oss << "    metalCommandQueue: " << static_cast<const void*>(metalCommandQueue) << '\n';
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
        if (toLower(backend) == "cpu") return Backend::CPU;
        if (toLower(backend) == "cuda") return Backend::CUDA;
        if (toLower(backend) == "metal") return Backend::METAL;
        return Backend::CPU;
    }

} // namespace dmxdenoiser
