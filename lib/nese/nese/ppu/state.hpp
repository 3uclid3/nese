#pragma once

#include <array>

#include <nese/cycle.hpp>

namespace nese::ppu {

using vram = std::array<byte_t, 0x4000>;
using oam = std::array<byte_t, 0x100>;

struct state
{
    vram vram{};
    oam oam{};
    cycle_t cycle{0};
};

} // namespace nese::ppu
