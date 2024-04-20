#pragma once

#include <memory>

#include <nese/basic_types.hpp>
#include <nese/cartridge_mapper/cartridge_mapper.hpp>
#include <nese/cartridge_mapper/cartridge_mapper_factory.hpp>

namespace nese {

class cartridge
{
public:
    static cartridge from_file(const char* filepath);

public:
    cartridge() = default;
    explicit cartridge(std::unique_ptr<cartridge_mapper>&& mapper);

    [[nodiscard]] const std::vector<byte_t>& get_prg() const;
    [[nodiscard]] const std::vector<byte_t>& get_chr() const;

    [[nodiscard]] byte_t read(addr_t addr) const;
    void write(addr_t addr, byte_t value);

private:
    std::unique_ptr<cartridge_mapper> _mapper;
};

inline const std::vector<byte_t>& cartridge::get_prg() const
{
    return _mapper->get_prg();
}

inline const std::vector<byte_t>& cartridge::get_chr() const
{
    return _mapper->get_chr();
}

} // namespace nese
