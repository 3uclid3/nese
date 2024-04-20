#include <nese/bus.hpp>

namespace nese::v2 {

namespace {

constexpr addr_t ram_addr_end{0x2000};
constexpr addr_t ppu_addr_end{0x4000};
constexpr addr_t apu_addr_end{0x4020};

[[nodiscard]] addr_t to_ram(addr_t addr)
{
    return addr & 0x07FF;
}

} // namespace

byte_t bus::read(addr_t addr) const
{
    if (addr < ram_addr_end)
    {
        return ram[to_ram(addr)];
    }
    else if (addr < ppu_addr_end)
    {
        // TODO read from ppu
        return 0xFF;
    }
    else if (addr < apu_addr_end)
    {
        // TODO read from apu
        return 0xFF;
    }
    else
    {
        return cartridge.read(addr);
    }
}

void bus::write(addr_t addr, byte_t value)
{
    if (addr < ram_addr_end)
    {
        ram[to_ram(addr)] = value;
    }
    else if (addr < ppu_addr_end)
    {
        // TODO write to ppu
    }
    else if (addr < apu_addr_end)
    {
        // TODO write to apu
    }
    else
    {
        cartridge.write(addr, value);
    }
}

} // namespace nese::v2