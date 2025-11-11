// AssertLogContains.hpp
#pragma once

#include <gtest/gtest.h>

#include <cassert>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>

namespace dmxdenoiser
{
    template<typename T>
    constexpr bool is_string_type =
        std::is_convertible_v<T, std::string> ||
        std::is_same_v<std::decay_t<T>, const char*>;

    template<typename... Args>
    void assertLogContains(std::string_view logFilePath, Args&&... args)
    {
        #if DMX_DEBUG_BUILD
            static_assert((is_string_type<Args> && ...), "All arguments must be std::string or const char*");

            ASSERT_TRUE(std::filesystem::exists(logFilePath));
            ASSERT_GT(std::filesystem::file_size(logFilePath), 0u);
            std::ifstream ifile{std::string(logFilePath)};
            ASSERT_TRUE(ifile.good());
            std::string logText{};
            std::string line{};
            while(std::getline(ifile, line))
                logText += line;
            (([&](){ EXPECT_NE(logText.find(std::forward<Args>(args)), std::string::npos); }() ), ...);
        #endif
    }

    template<typename... Args>
    void assertLogDoesNotContain(std::string_view logFilePath, Args&&... args)
    {
        #if DMX_DEBUG_BUILD
            static_assert((is_string_type<Args> && ...), "All arguments must be std::string or const char*");
        
            ASSERT_TRUE(std::filesystem::exists(logFilePath));
            ASSERT_GT(std::filesystem::file_size(logFilePath), 0u);
            std::ifstream ifile{std::string(logFilePath)};
            ASSERT_TRUE(ifile.good());
            std::string logText{};
            std::string line{};
            while(std::getline(ifile, line))
                logText += line;
            (([&](){ EXPECT_EQ(logText.find(std::forward<Args>(args)), 
                std::string::npos) << "Log contains substring '"
                << std::forward<Args>(args) << "' (should be absent)"; }() ), ...);
        #endif
    }

} // namespace dmxdenoiser
