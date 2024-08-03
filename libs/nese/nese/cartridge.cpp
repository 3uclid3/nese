#include <nese/cartridge.hpp>

#include <fstream>

#include <nese/cartridge/cartridge_mapper_factory.hpp>
#include <nese/utility/assert.hpp>
#include <nese/utility/log.hpp>

namespace nese {

struct cartridge_header
{
    char magic[4]; // 0x4E, 0x45, 0x53, 0x1A
    u8_t prg_size; // PRG ROM in 16K
    u8_t chr_size; // CHR ROM in 8K, 0 -> using CHR RAM
    u8_t flag6;
    u8_t flag7;
    u8_t prg_ram_size; // PRG RAM in 8K
    u8_t flag9;
    u8_t flag10;      // unofficial
    char reserved[5]; // reserved
};

static_assert(sizeof(cartridge_header) == 0x10);

cartridge cartridge::from_file(const char* filepath)
{
    NESE_TRACE("[cartridge] opening file '{}'", filepath);

    std::ifstream file;
    file.open(filepath, std::ifstream::in | std::ifstream::binary);

    cartridge_header header;
    file.read(reinterpret_cast<char*>(&header), sizeof(cartridge_header));

    if (header.flag6 & 0x4)
    {
        NESE_TRACE("[cartridge] Skipping trainer bytes");

        // skip the 512-byte trainer
        file.seekg(std::ios_base::cur, 0x200);
    }

    const byte_t mapper_id = ((header.flag6 & 0xf0) >> 4) + (header.flag7 & 0xf0);

    NESE_TRACE("[cartridge] mapper id = {}", mapper_id);

    const size_t prg_rom_size = static_cast<size_t>(header.prg_size * 0x4000); // 16KB
    const size_t chr_rom_size = static_cast<size_t>(header.chr_size * 0x2000); // 4KB

    std::vector<byte_t> prg;
    prg.resize(prg_rom_size);

    std::vector<byte_t> chr;
    chr.resize(chr_rom_size);

    file.read(reinterpret_cast<char*>(prg.data()), static_cast<std::streamsize>(prg_rom_size));
    file.read(reinterpret_cast<char*>(chr.data()), static_cast<std::streamsize>(chr_rom_size));

    file.close();

    const cartridge_mapper::mirroring mirroring = header.flag6 & 0x01 ? cartridge_mapper::mirroring::vertical : cartridge_mapper::mirroring::horizontal;

    return cartridge{cartridge_mapper_factory::create(mapper_id, std::move(prg), std::move(chr), mirroring)};
}

cartridge::cartridge(std::unique_ptr<cartridge_mapper>&& mapper)
    : _mapper(std::move(mapper))
{
}

byte_t cartridge::read(addr_t addr) const
{
    NESE_ASSERT(_mapper);
    return _mapper->read(addr);
}

void cartridge::write(addr_t addr, byte_t value)
{
    NESE_ASSERT(_mapper);
    _mapper->write(addr, value);
}

} // namespace nese
