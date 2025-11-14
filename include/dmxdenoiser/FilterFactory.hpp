#ifndef DMXDENOISER_FILTER_FACTORY_H
#define DMXDENOISER_FILTER_FACTORY_H

#include <dmxdenoiser/Filter.hpp>
#include <dmxdenoiser/Logger.hpp>
#include <dmxdenoiser/ParamDictionary.hpp>

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>

/// Macro to automatically register a filter class with the global FilterFactory.
/// Usage: Place REGISTER_FILTER(YourFilterClass) in your filter's .cpp file.
/// This ensures the filter is registered before main() using a static object,
/// so it can be created by name at runtime (for plugins, configs, etc).
///
/// The anonymous namespace and static object ensure registration is safe and unique
/// per translation unit, preventing linker conflicts or double registration.
#define REGISTER_FILTER(CLASS) \
    namespace{ /* Anonymous namespace gives internal linkage to avoid symbol conflicts */ \
        struct CLASS##Registrator{ \
            CLASS##Registrator() { \
                /* Registration happens in the constructor, which is called before main() */ \
                dmxdenoiser::FilterFactory::instance().registerFilter( \
                    CLASS::StaticClassName(), /* Register filter under its static name */ \
                    [](){ return std::make_unique<CLASS>(); }); /* Lambda creates new instance */ \
            } \
        }; \
        /* Object in anonymous namespace guarantees constructor runs for each translation unit before main(). */ \
        /* In C++11 and later, `static` is not required here—anonymous namespace is sufficient. */ \
        static CLASS##Registrator global_##CLASS##Registrator; \
    } \
    

namespace dmxdenoiser
{
    /// Singleton factory for registering and creating filters
    class FilterFactory
    {
    public:
        using Creator = std::function<std::unique_ptr<Filter>()>;

        static FilterFactory& instance()
        {
            static FilterFactory f;
            return f;
        }

        /// Register a filter by name
        void registerFilter(const std::string& name, Creator creator)
        {
            m_creators[name] = std::move(creator);
        }

        /// Create filter by name
        std::unique_ptr<Filter> create(
            const std::string& name,
            const ParamDictionary* params = nullptr) const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_creators.find(name);
            if (it != m_creators.end())
            {
                auto filter = it->second();
                if(params)
                    filter->setParams(*params);
                return filter;
            }
            DMX_LOG_ERROR("FilterFactory", "Filter not registered: " + name);
            throw std::runtime_error("Filter not registered: " + name);
        }

    private:
        mutable std::mutex m_mutex;
        std::unordered_map<std::string, Creator> m_creators;

        /// Singleton: prevent copying
        FilterFactory() = default;
        FilterFactory(const FilterFactory&) = delete;
        FilterFactory& operator=(const FilterFactory&) = delete;
    };

} // namespace dmxdenoiser

// Example: DMX_CREATE_FILTER("ConvolutionFilter", nullptr);
#define DMX_CREATE_FILTER(NAME, PARAMS) FilterFactory::instance().create(NAME, PARAMS)

#endif // DMXDENOISER_FILTER_FACTORY_H
