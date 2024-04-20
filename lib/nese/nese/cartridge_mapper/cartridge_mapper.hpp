#pragma once

#include <nese/basic_types.hpp>

namespace nese {

class cartridge_mapper
{
public:
    enum class mirroring : byte_t
    {
        horizontal,
        vertical
    };

    using id = byte_t;

public:
    cartridge_mapper(std::vector<byte_t>&& prg, std::vector<byte_t>&& chr);
    virtual ~cartridge_mapper() = default;

    [[nodiscard]] const std::vector<byte_t>& get_prg() const;
    [[nodiscard]] const std::vector<byte_t>& get_chr() const;

    [[nodiscard]] virtual byte_t read(addr_t addr) const = 0;
    virtual void write(addr_t addr, byte_t value) = 0;

private:
    std::vector<byte_t> _prg;
    std::vector<byte_t> _chr;
};

inline cartridge_mapper::cartridge_mapper(std::vector<byte_t>&& prg, std::vector<byte_t>&& chr)
    : _prg(std::move(prg))
    , _chr(std::move(chr))
{
}

inline const std::vector<byte_t>& cartridge_mapper::get_prg() const
{
    return _prg;
}

inline const std::vector<byte_t>& cartridge_mapper::get_chr() const
{
    return _chr;
}

} // namespace nese
