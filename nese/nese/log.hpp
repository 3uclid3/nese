#pragma once

#include <spdlog/spdlog.h>

#define NESE_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define NESE_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define NESE_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define NESE_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define NESE_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define NESE_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

#define NESE_CRITICAL_ASSERT(condition, ...) \
    do { \
        const bool result = (condition); \
        if (!result) \
        { \
            SPDLOG_CRITICAL("[assert] condition: {}", #condition); \
            SPDLOG_CRITICAL(__VA_ARGS__); \
            assert(false); \
        } \
    } while (false)
