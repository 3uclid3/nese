#pragma once

#include <array>

#include <nese/basic_types.hpp>

namespace nese {

class ram
{
public:
    static constexpr auto max_size = 0x10000;

public:
    void power_on();
    void reset();

public:
    [[nodiscard]] byte_t get_byte(addr_t addr) const;
    void set_byte(addr_t addr, byte_t val);

    [[nodiscard]] word_t get_word(addr_t addr) const;
    void set_word(addr_t addr, word_t val);

    void set_bytes(addr_t addr, const byte_t* data, size_t size);

private:
    [[nodiscard]] static addr_t redirect_addr(addr_t addr);

private:
    std::array<byte_t, max_size> _data{};
};

} // namespace nese
