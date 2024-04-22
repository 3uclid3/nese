#pragma once

#include <nese/basic_types.hpp>
#include <nese/cartridge.hpp>
#include <nese/cpu.hpp>
#include <nese/ppu.hpp>

namespace nese {

struct bus
{
    [[nodiscard]] byte_t readonly(addr_t addr) const;
    [[nodiscard]] byte_t read(addr_t addr);
    void write(addr_t addr, byte_t value);

    [[nodiscard]] word_t readonly_word(addr_t addr) const;
    [[nodiscard]] word_t read_word(addr_t addr);
    void write_word(addr_t addr, word_t value);

    array<byte_t, 2048> ram{};
    cartridge cartridge{};
    cpu<bus> cpu{*this};
    ppu<bus> ppu{*this};
};

inline word_t bus::readonly_word(addr_t addr) const
{
    return readonly(addr) + static_cast<word_t>(static_cast<word_t>(readonly(addr + 1)) << 8);
}

inline word_t bus::read_word(addr_t addr)
{
    return read(addr) + static_cast<word_t>(static_cast<word_t>(read(addr + 1)) << 8);
}

inline void bus::write_word(addr_t addr, word_t value)
{
    write(addr, static_cast<byte_t>(value & 0xff));
    write(addr + 1, static_cast<byte_t>(value >> 8));
}

} // namespace nese
