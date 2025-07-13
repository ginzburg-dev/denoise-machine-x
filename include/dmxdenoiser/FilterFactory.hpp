#ifndef DMXDENOISER_FILTER_FACTORY_H
#define DMXDENOISER_FILTER_FACTORY_H

#include <functional>
#include <map>
#include <memory>

namespace dmxdenoiser
{

    class FilterFactory
    {
        using Creator = std:function<>

        static FilterFactory& instance()
        {
            static FilterFactory f;
            return f;
        }


    };

} // namespace dmxdenoiser

#endif // DMXDENOISER_FILTER_FACTORY_H
