#pragma once

#include <nese/basic_types.hpp>
#include <nese/utility/details/crc32_table.hpp>

namespace nese {

constexpr u32_t crc32(string_view string, u32_t crc = 0u)
{
    crc = crc ^ 0xFFFFFFFFu;
    for (const char c : string)
    {
        crc = details::crc32_table[static_cast<u32_t>(c) ^ (crc & 0xFF)] ^ (crc >> 8);
    }
    crc = crc ^ 0xFFFFFFFFu;
    return crc;
}

} // namespace nese