#include <nese/memory/mapper.hpp>

#include <cstring>

#include <nese/utility/assert.hpp>

namespace nese::memory {

void mapper::set_zero()
{
    std::memset(_data.data(), 0, _data.size());
}

byte_t mapper::get_byte(addr_t addr) const
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

void mapper::set_byte(addr_t addr, byte_t value)
{
    addr = redirect_addr(addr);

    if ((addr & 0xfff8) == 0x2000 || (addr & 0xffe0) == 0x4000)
    {
        // TODO IO reg
        // NESE_ASSERT(false);
        // return;
    }

    _data[addr] = value;
}

word_t mapper::get_word(addr_t addr) const
{
    NESE_ASSERT(addr <= 0xFFFE);

    addr = redirect_addr(addr);

    // NES 6502 CPU is little endian
    return get_byte(addr) + static_cast<word_t>(static_cast<word_t>(get_byte(addr + 1)) << 8);
}

void mapper::set_word(addr_t addr, word_t value)
{
    NESE_ASSERT(addr <= 0xFFFE);

    addr = redirect_addr(addr);

    // NES 6502 CPU is little endian
    set_byte(addr, value & 0xff);
    set_byte(addr + 1, value >> 8);
}

std::span<const byte_t> mapper::get_bytes(addr_t addr) const
{
    addr = redirect_addr(addr);

    return {&_data[addr], _data.size() - addr};
}

void mapper::set_bytes(addr_t addr, const byte_t* data, size_t size)
{
    NESE_ASSERT(redirect_addr(addr) + size <= _data.size());

    std::memcpy(_data.data() + redirect_addr(addr), data, size);
}

addr_t mapper::redirect_addr(addr_t addr)
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

//bool mapper::is_io_registers(addr_t addr)
//{
//    return (addr & 0xFFF8) == 0x2000 || (addr & 0xFFE0) == 0x4000;
//}

} // namespace nese::memory
