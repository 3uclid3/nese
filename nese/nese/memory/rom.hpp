#pragma once

#include <vector>

#include <nese/basic_types.hpp>

namespace nese::memory {

class rom
{
public:
    struct header
    {
        u8_t magic[4]; // 0x4E, 0x45, 0x53, 0x1A
        u8_t prg_size; // PRG ROM in 16K
        u8_t chr_size; // CHR ROM in 8K, 0 -> using CHR RAM
        u8_t flag6;
        u8_t flag7;
        u8_t prg_ram_size; // PRG RAM in 8K
        u8_t flag9;
        u8_t flag10;      // unofficial
        u8_t reserved[5]; // reserved
    };

    using prg = std::vector<byte_t>;
    using chr = std::vector<byte_t>;

    static rom from_file(const char* path);

public:
    const header& get_header() const;
    const prg& get_prg() const;
    const chr& get_chr() const;

    u8_t get_mapper() const;

private:
    rom() = default;

private:
    header _header;
    prg _prg;
    chr _chr;
};

inline const rom::header& rom::get_header() const
{
    return _header;
}

inline const rom::prg& rom::get_prg() const
{
    return _prg;
}

inline const rom::chr& rom::get_chr() const
{
    return _chr;
}

} // namespace nese::memory
