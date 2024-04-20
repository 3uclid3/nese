#pragma once

#include <nese/basic_types.hpp>
#include <nese/cartridge.hpp>
#include <nese/cpu.hpp>

namespace nese::v2 {

struct bus
{
    [[nodiscard]] byte_t read(addr_t addr) const;
    void write(addr_t addr, byte_t value);

    [[nodiscard]] word_t read_word(addr_t addr) const;
    void write_word(addr_t addr, word_t value);

    array<byte_t, 2048> ram{};
    cartridge cartridge{};
    cpu<bus> cpu{*this};
};

inline word_t bus::read_word(addr_t addr) const
{
    return read(addr) + static_cast<word_t>(static_cast<word_t>(read(addr + 1)) << 8);
}

inline void bus::write_word(addr_t addr, word_t value)
{
    write(addr, static_cast<byte_t>(value & 0xff));
    write(addr + 1, static_cast<byte_t>(value >> 8));
}


} // namespace nese
