// ChannelBuffer.hpp
#pragma once

#include <dmxdenoiser/DMXImage.hpp>
#include <dmxdenoiser/PixelType.hpp>

#include <cstddef>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include <Imath/half.h>

namespace dmxdenoiser
{

    struct PixelDataContainer
    {
        std::vector<uint8_t> uint8s{};
        std::vector<uint16_t> uint16s{};
        std::vector<uint32_t> uint32s{};
        std::vector<half> halfs{};
        std::vector<float> floats{};
        std::vector<double> doubles{};

        PixelDataContainer() = default;
        PixelDataContainer(PixelType type, std::size_t size);

        void allocate(PixelType type, std::size_t size);
        void clear();
    };

    class ChannelBuffer
    {
    public:
        ChannelBuffer() = default;
        ChannelBuffer(const std::string& name, PixelType pixelType, std::size_t size);

        ChannelBuffer(const ChannelBuffer&) noexcept = delete;
        ChannelBuffer operator=(const ChannelBuffer&) noexcept = delete;

        ChannelBuffer(ChannelBuffer&&) = default;
        ChannelBuffer& operator=(ChannelBuffer&&) = default;

        ~ChannelBuffer() = default;

        void initialize(const std::string& name, PixelType pixelType, std::size_t size);

        const std::string& getName() const { return m_name; };
        PixelType getPixelType() const { return m_pixelType; };

        char* getRawPtr();
        const char* getRawPtr() const;

        PixelDataContainer& data() { return m_data; }

        void clear() { m_data.clear(); }

    private:
        std::string m_name{};
        PixelType m_pixelType = PixelType::Unknown;
        PixelDataContainer m_data;
    };

    /// @brief Stores per-layer channel buffers: layer name -> list of ChannelBuffer (e.g. R, G, B, A)
    struct LayerChannelBufferMap
    {
        std::map<std::string, std::vector<ChannelBuffer>> buffers;

        void set(const std::string& name, ChannelBuffer&& buf);
        void set(const std::string& layerName, const std::string& channelName, PixelType pixelType, std::size_t size);

        bool hasLayer(const std::string& name);
        bool hasBuffer(const std::string& layerName, const std::string& bufferName);

        std::vector<ChannelBuffer>* getLayerBuffers(const std::string& name);
        ChannelBuffer* getBuffer(const std::string& layerName, const std::string& channelName);
    };

    void copyChannelBuffersToDMXImage(
        const std::vector<ChannelBuffer>& buff,
        std::string_view layer, 
        int frame, 
        DMXImage& img
    );

    std::vector<ChannelBuffer> copyDMXImageToChannelBuffers(
        const DMXImage& img,
        const std::vector<std::string>& layers,
        int frame = 0
    );
    
} // namespace dmxdenoiser
