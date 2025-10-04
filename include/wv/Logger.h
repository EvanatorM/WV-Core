#pragma once

#include <cstdio>
#include <ctime>

#define LOG_COLOR_RESET        "\x1b[0m"
#define LOG_COLOR_APP_WARN     "\x1b[93m"
#define LOG_COLOR_APP_ERROR    "\x1b[91m"
#define LOG_COLOR_ENGINE_LOG   "\x1b[32m"
#define LOG_COLOR_ENGINE_WARN  "\x1b[33m"
#define LOG_COLOR_ENGINE_ERROR "\x1b[41m"

namespace WillowVox
{
    class Logger
    {
    public:
        template <typename... Args>
        static void Log(const char* message, Args... args)
        {
            std::time_t t = std::time(nullptr);
            std::tm now;

            #ifdef _WIN32
            localtime_s(&now, &t);
            #else
            localtime_r(&t, &now);
            #endif

            printf(LOG_COLOR_RESET "[%02d:%02d:%02d] ", now.tm_hour, now.tm_min, now.tm_sec);
            printf(message, args...);
            printf(LOG_COLOR_RESET "\n");
        }
    };
}