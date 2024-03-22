#include <nese/memory/rom.hpp>

#include <fstream>

#include <nese/log.hpp>

namespace nese::memory {

static_assert(sizeof(rom::header) == 0x10);

rom rom::from_file(const char* path)
{
    NESE_TRACE("[rom] opening file '{}'", path);

    std::ifstream file;
    file.open(path, std::ifstream::in | std::ifstream::binary);

    rom loaded_rom;
    file.read(reinterpret_cast<char*>(&loaded_rom._header), sizeof(header));

    if (loaded_rom._header.flag6 & 0x4)
    {
        NESE_TRACE("[rom] Skipping trainer bytes");

        // skip the 512-byte trainer
        file.seekg(std::ios_base::cur, 0x200);
    }

    NESE_TRACE("[rom] header flag6 = {:x}, flag7 = {:x}, mapper_id={}", loaded_rom._header.flag6, loaded_rom._header.flag7, loaded_rom.get_mapper());

    const size_t prg_rom_size = static_cast<size_t>(loaded_rom._header.prg_size * 0x4000); // 16KB
    const size_t chr_rom_size = static_cast<size_t>(loaded_rom._header.chr_size * 0x2000); // 4KB

    loaded_rom._prg.resize(prg_rom_size);
    loaded_rom._chr.resize(chr_rom_size);

    file.read(reinterpret_cast<char*>(loaded_rom._prg.data()), static_cast<std::streamsize>(prg_rom_size));
    file.read(reinterpret_cast<char*>(loaded_rom._chr.data()), static_cast<std::streamsize>(chr_rom_size));

    file.close();

    return loaded_rom;
}

u8_t rom::get_mapper() const
{
    return ((_header.flag6 & 0xf0) >> 4) + (_header.flag7 & 0xf0);
}

} // namespace nese::memory
