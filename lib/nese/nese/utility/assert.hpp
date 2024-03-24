#pragma once

#ifndef NESE_ASSERT_ENABLED
#ifdef _DEBUG
#define NESE_ASSERT_ENABLED 1
#else
#define NESE_ASSERT_ENABLED 0
#endif
#endif

#define NESE_ASSUME_IMPL(condition) __builtin_assume(condition)
#define NESE_UNREACHABLE_IMPL() __builtin_unreachable()

#if NESE_ASSERT_ENABLED

#include <chrono>
#include <thread>

#include <nese/utility/log.hpp>

#define NESE_ASSERT(condition) \
    do { \
        const bool result = (condition); \
        if (!result) \
        { \
            SPDLOG_CRITICAL("[assert] condition: {}", #condition); \
            nese::debug_trigger_assert(); \
        } \
    } while (false)

#define NESE_ASSERT_LOG(condition, ...) \
    do { \
        const bool result = (condition); \
        if (!result) \
        { \
            SPDLOG_CRITICAL("[assert] condition: {}", #condition); \
            SPDLOG_CRITICAL(__VA_ARGS__); \
            nese::debug_trigger_assert(); \
        } \
    } while (false)


#define NESE_ASSUME(condition) \
    NESE_ASSERT(condition); \
    NESE_ASSUME_IMPL(condition)

#define NESE_ASSUME_LOG(condition, ...) \
    NESE_ASSERT_LOG(condition, __VA_ARGS__); \
    NESE_ASSUME_IMPL(condition)

#define NESE_UNREACHABLE() \
    NESE_ASSERT(false); \
    NESE_UNREACHABLE_IMPL()

#define NESE_UNREACHABLE_LOG(...) \
    NESE_ASSERT_LOG(false, __VA_ARGS__); \
    NESE_UNREACHABLE_IMPL()

namespace nese {

inline void debug_trigger_assert()
{ 
    assert(false); // TODO Figure out why nintendulator is not flushed properly

    // spdlog::drop_all();
    // spdlog::shutdown();
    //
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //
    // #if __has_builtin(__builtin_trap)
    //    __builtin_trap();
    // #else
    // #error Unsupported plateform
    // #endif
}

} // namespace nese

#else

#define NESE_ASSERT(...)
#define NESE_ASSERT_LOG(...)

#define NESE_ASSUME(condition) NESE_ASSUME_IMPL(condition)
#define NESE_ASSUME_LOG(condition, ...) NESE_ASSUME_IMPL(condition)

#define NESE_UNREACHABLE() NESE_UNREACHABLE_IMPL()
#define NESE_UNREACHABLE_LOG(...) NESE_UNREACHABLE_IMPL()

namespace nese {

constexpr void debug_trigger_assert()
{
}

} // namespace nese

#endif
