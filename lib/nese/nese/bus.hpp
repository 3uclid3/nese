#pragma once

#include <array>

#include <nese/cpu/state.hpp>
#include <nese/ppu/state.hpp>
#include <nese/memory/ram.hpp>

namespace nese {

struct bus
{
    byte_t read_byte(addr_t addr);
    void write_byte(addr_t addr, byte_t byte);

    word_t read_word(addr_t addr);
    void write_word(addr_t addr, word_t byte);

    cpu::state cpu;
    ppu::state ppu;

    memory::ram ram;
};

} // namespace nese
