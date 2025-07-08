#include <dmxdenoiser/image/DMXImage.hpp>

namespace dmxdenoiser
{
    /*
    Image& ImageCollection::getImage(std::string_view channelName, int frame)
    {
        return m_data[std::string(channelName)][frame];
    }

    Pixel& ImageCollection::at(std::string_view channelName, int frame, int x, int y)
    {
        return m_data[std::string(channelName)][frame].at(x, y);
    }

    bool ImageCollection::addImage(std::string_view channelName, int frame, Image&& img)
    {
        if ( m_data.size() < frame )
        {
            m_data[std::string(channelName)][frame] = std::move(img);
            return true;
        } 
        else
            return false;
    }

    bool ImageCollection::addImage(std::string_view channelName, Image&& img)
    {
        m_data[std::string(channelName)].push_back(std::move(img));
        return true;
    }
        */
}
