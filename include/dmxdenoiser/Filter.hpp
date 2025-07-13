#ifndef DMXDENOISER_FILTER_H
#define DMXDENOISER_FILTER_H

#include <dmxdenoiser/DMXImage.hpp>

#include <string>
#include <string_view>
#include <unordered_map>

namespace dmxdenoiser
{

    using FilterParamMap = std::unordered_map<std::string, float>;
    using FilterMap = std::map<std::string, FilterParamMap>;

    /*Abstract*/
    class Filter
    {
    public:
        Filter() = default;
        virtual const char* Name() const = 0;
        virtual void apply(DMXImage& img) const;
        virtual void setParam(const FilterParamMap& params) { m_params = param; };
        virtual void setParam(const std::string& key, float value) { m_params[key] = value; };
        virtual float getParam(const std::string& key)
        {
            auto it = m_params.find(key);
            if (it != m_params.end())
                return it->second;
            throw std::runtime_error("Parameter not found: " + key);
        };
        virtual FilterParamMap getParamMap() const { return m_params; };
        virtual std::string ToString() const = 0; ///< String representation of the filter name and parameters for logging.
        virtual ~Filter() = default;

    protected:
        FilterParamMap m_params;
    };

} // namespace dmxdenoiser

#endif // DMXDENOISER_FILTER_H
