#pragma once

#include <numeric>

#include <nese/basic_types.hpp>
#include <nese/utility/details/crc32_table.hpp>

namespace nese {

constexpr u32_t crc32(string_view string, u32_t crc = 0u)
{
    return std::accumulate(string.begin(), string.end(), crc, [](uint32_t current_crc, char c) {
               return details::crc32_table[static_cast<uint8_t>(c) ^ (current_crc & 0xFF)] ^ (current_crc >> 8);
           }) ^ 0xFFFFFFFFu;
}

} // namespace nese