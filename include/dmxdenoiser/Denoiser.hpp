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
        explicit Denoiser(const Options& opt);
        void init(const Options& opt);

        void run();

        ~Denoiser() = default;

    private:
        Options m_options{};
        std::vector<std::unique_ptr<Filter>> m_filters{};

    }

} // namespace dmxdenoiser
