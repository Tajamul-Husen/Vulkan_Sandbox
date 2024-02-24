#pragma once

#include <spdlog/spdlog.h>

#if _MSC_VER
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

// Logging macros
#define CORE_LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define CORE_LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define CORE_LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
// #define CORE_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)

#ifdef VKS_DEBUG
#define CORE_ASSERT(check, ...)                                   \
    {                                                             \
        if (!(check))                                             \
        {                                                         \
            CORE_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
            debugBreak();                                         \
        }                                                         \
    }
#else
#define CORE_ASSERT(check, ...)
#endif
