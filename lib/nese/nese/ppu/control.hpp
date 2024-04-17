#pragma once

namespace nese::ppu {

enum class control
{
    PpuControlRegister = 0x2000,          // PPUCTRL
    PpuMaskRegister = 0x2001,             // PPUMASK
    PpuStatusRegister = 0x2002,           // PPUSTATUS
    SpriteMemoryAddressRegister = 0x2003, // OAMADDR
    SpriteMemoryDataRegister = 0x2004,    // OAMDATA
    StrollPositionRegister = 0x2005,      // PPUSCROLL
    VramAddressRegister = 0x2006,         // OAMDATA
    VramDataRegister = 0x2007,            // PPUDATA
    SpriteDmaRegister = 0x2004,           // OAMDMA
};

}
