// Denoiser.hpp
#pragma once

#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/Options.hpp>

#include <vector>
#include <memory>

namespace dmxdenoiser
{

    class Denoiser
    {
    public:
        Denoiser() = default;
        Denoiser(const Options& opt);
        ~Denoiser() = default;

        void init(const Options& opt);

    private:
        std::vector<std::unique_ptr<Filter>> filters{};
        std::vector<int> frames{};
        std::vector<std::string> layers{};

    }

} // namespace dmxdenoiser
