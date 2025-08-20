// Logger.hpp
#pragma once

#include <dmxdenoiser/Config.hpp>
#include <dmxdenoiser/util/FileUtils.hpp>

#include <ctime>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace dmxdenoiser
{

    enum class LogLevel { Trace, Debug, Info, Warning, Error, Critical};

    inline constexpr std::string_view ToString(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default: return "Unknown log level";
        }
        return "Unknown log level";
    }

    class Logger
    {
    public:
        static Logger& instance() {
            static Logger inst;
            return inst;
        }
        
        void init(LogLevel minLevel, std::ostream* os = nullptr, const std::string& filePath = "") {
            std::lock_guard<std::mutex> lock{m_mutex};
            m_minLevel = static_cast<int>(minLevel);
            m_filePath = filePath;
            m_out = os;
            if (m_fileStream.is_open())       // close old file if any
                m_fileStream.close();

            if (!filePath.empty()) {
                m_fileStream.open(filePath, std::ios::out | std::ios::app);
            }
        }

        template<typename... Args>
        void Log(LogLevel level, std::string_view tag, Args&&... args) {
            if (static_cast<int>(level) < m_minLevel)
                return;
            auto now = std::chrono::system_clock::now();
            auto t = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
            #if defined(_WIN32)
                localtime_s(&tm, &t);
            #else
                localtime_r(&t, &tm);
            #endif
            char buf[24];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);

            // Build message text once (handles empty varargs too), with neat spacing
            std::ostringstream msg;
            const char* sep = "";
            (((msg << std::forward<Args>(args) << sep), sep = ""), ...);
            const std::string line = msg.str();

            std::lock_guard<std::mutex> lock(m_mutex);
            
            auto out_one = [&](std::ostream& os) {
                os  << "[" << buf << "]: "
                    << "[" << ToString(level) << "] "
                    << "[" << tag << "] "
                    << line << '\n';
            };

            if (m_out) out_one(*m_out);
            if (m_fileStream.is_open()) { out_one(m_fileStream); m_fileStream.flush(); }
        }

        ~Logger() noexcept { m_fileStream.close(); }

    private:
        std::string m_filePath{};
        std::mutex m_mutex;
        std::ostream* m_out = nullptr;
        std::ofstream m_fileStream;
        int m_minLevel{ static_cast<int>(LogLevel::Info) };

        Logger() = default;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(Logger&&) = delete;

    };

} // namespace dmxdenoiser

// Initialize logger
#define DMX_LOG_INIT(LOGLEVEL, OSTREAMPTR, FILENAME) \
    dmxdenoiser::Logger::instance().init((LOGLEVEL), (OSTREAMPTR), (FILENAME)); \
    dmxdenoiser::Logger::instance().Log(dmxdenoiser::LogLevel::Info, "Logger", "Logging system initialized")

#if DMX_DEBUG_BUILD
    #define DMX_LOG_TRACE(tag, ...) dmxdenoiser::Logger::instance().Log(dmxdenoiser::LogLevel::Trace, (tag), ##__VA_ARGS__)
#else
    #define DMX_LOG_TRACE(tag, ...) ((void)0)
#endif

#if DMX_DEBUG_BUILD
    #define DMX_LOG_DEBUG(tag, ...) dmxdenoiser::Logger::instance().Log(dmxdenoiser::LogLevel::Debug, (tag), ##__VA_ARGS__)
#else
    #define DMX_LOG_DEBUG(tag, ...) ((void)0)
#endif

#define DMX_LOG_INFO(tag, ...) dmxdenoiser::Logger::instance().Log(dmxdenoiser::LogLevel::Info, (tag), ##__VA_ARGS__)
#define DMX_LOG_WARNING(tag, ...) dmxdenoiser::Logger::instance().Log(dmxdenoiser::LogLevel::Warning, (tag), ##__VA_ARGS__)
#define DMX_LOG_ERROR(tag, ...) dmxdenoiser::Logger::instance().Log(dmxdenoiser::LogLevel::Error, (tag), ##__VA_ARGS__)
#define DMX_LOG_CRITICAL(tag, ...) dmxdenoiser::Logger::instance().Log(dmxdenoiser::LogLevel::Critical, (tag), ##__VA_ARGS__)
