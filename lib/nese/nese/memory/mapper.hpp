#pragma once

#include <array>
#include <span>

#include <nese/basic_types.hpp>

namespace nese::memory {

class mapper
{
public:
    static constexpr auto capacity = 0x10000;

public:
    void set_zero();

    [[nodiscard]] byte_t get_byte(addr_t addr) const;
    void set_byte(addr_t addr, byte_t value);

    [[nodiscard]] word_t get_word(addr_t addr) const;
    void set_word(addr_t addr, word_t value);

    std::span<const byte_t> get_bytes(addr_t addr = null_addr) const;
    void set_bytes(addr_t addr, const byte_t* data, size_t size);

private:
    [[nodiscard]] static addr_t redirect_addr(addr_t addr);

private:
    std::array<byte_t, capacity> _data{};
};

} // namespace nese::memory
