#pragma once

#include <spdlog/spdlog.h>

#define NESE_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define NESE_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define NESE_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define NESE_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define NESE_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define NESE_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
