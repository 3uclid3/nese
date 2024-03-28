#include <nese/memory/ram.hpp>

#include <nese/utility/assert.hpp>

namespace nese::memory {

void ram::power_on()
{
    std::memset(_data.data(), 0, max_size);
}

void ram::reset()
{
}

byte_t ram::get_byte(addr_t addr) const
{
    addr = redirect_addr(addr);

    if ((addr & 0xfff8) == 0x2000 || (addr & 0xffe0) == 0x4000)
    {
        // TODO IO reg
        // return;
        // NESE_ASSERT(false);
    }

    return _data[addr];
}

void ram::set_byte(addr_t addr, byte_t value)
{
    addr = redirect_addr(addr);

    if ((addr & 0xfff8) == 0x2000 || (addr & 0xffe0) == 0x4000)
    {
        // TODO IO reg
        // NESE_ASSERT(false);
        return;
    }

    _data[addr] = value;
}

word_t ram::get_word(addr_t addr) const
{
    addr = redirect_addr(addr);

    // NES 6502 CPU is little endian
    return get_byte(addr) + static_cast<word_t>(static_cast<word_t>(get_byte(addr + 1)) << 8);
}

void ram::set_word(addr_t addr, word_t value)
{
    addr = redirect_addr(addr);

    // NES 6502 CPU is little endian
    set_byte(addr, value & 0xff);
    set_byte(addr + 1, value >> 8);
}

void ram::set_bytes(addr_t addr, const byte_t* data, size_t size)
{
    NESE_ASSERT(addr + size <= max_size);

    std::memcpy(_data.data() + redirect_addr(addr), data, size);
}

addr_t ram::redirect_addr(addr_t addr)
{
    if ((addr & 0xE000) == 0)
    {
        // map 0x0000~0x07ff 4 times until 0x1ffff
        return addr & 0x7ff;
    }

    if ((addr & 0xE000) == 0x2000)
    {
        // map 0x2000~0x2008 every 8 bytes until 0x3ffff
        return addr & 0x2007;
    }

    return addr;
}

} // namespace nese::memory
