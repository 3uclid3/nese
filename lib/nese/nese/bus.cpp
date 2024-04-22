#include <nese/bus.hpp>

namespace nese {

namespace {

constexpr addr_t ram_addr_end{0x2000};
constexpr addr_t ppu_addr_end{0x4000};
constexpr addr_t apu_addr_end{0x4020};

[[nodiscard]] addr_t to_ram(addr_t addr)
{
    return addr & 0x07FF;
}

} // namespace

enum class ss
{
    control = 0,
    mask = 1,
    status = 2,
    oam_addr = 3,
    oam_data = 4,
    scroll = 5,
    ppu_addr = 6,
    ppu_data = 7
};

byte_t bus::readonly(addr_t addr) const
{
    if (addr < ram_addr_end)
    {
        return ram[to_ram(addr)];
    }
    else if (addr < ppu_addr_end)
    {
        switch (static_cast<ss>(addr & 0x0007))
        {
        case ss::scroll:
        case ss::ppu_addr:
        case ss::ppu_data:
            // Unreadable
            return 0xFF;

        case ss::oam_addr:
            return 0xFF;

        case ss::oam_data:
            return 0xFF;

        case ss::control:
            return ppu.read_control();

        case ss::mask:
            return ppu.read_mask();

        case ss::status:
            return ppu.read_status();
        }

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

byte_t bus::read(addr_t addr)
{
    if (addr < ram_addr_end)
    {
        return ram[to_ram(addr)];
    }
    else if (addr < ppu_addr_end)
    {
        switch (static_cast<ss>(addr & 0x0007))
        {
        case ss::control:
        case ss::mask:
        case ss::scroll:
        case ss::ppu_addr:
            // Unreadable
            return 0xFF;

        case ss::oam_addr:
            // TODO
            return 0xFF;

        case ss::oam_data:
            // TODO
            return 0xFF;

        case ss::status:
            return ppu.read_status();

        case ss::ppu_data:
            return ppu.read_ppu_data();
        }

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
        switch (static_cast<ss>(addr & 0x0007))
        {
        case ss::control:
            ppu.write_control(value);
            break;

        case ss::mask:
            ppu.write_mask(value);
            break;

        case ss::scroll:
            ppu.write_scroll(value);
            break;

        case ss::ppu_addr:
            ppu.write_ppu_address(value);
            break;

        case ss::ppu_data:
            ppu.write_ppu_data(value);
            break;

        case ss::status:
        case ss::oam_addr:
        case ss::oam_data:
            // TODO
            break;
        }
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

} // namespace nese