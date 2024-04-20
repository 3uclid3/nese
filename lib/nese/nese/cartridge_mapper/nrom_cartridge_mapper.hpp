#pragma once

#include <vector>

#include <nese/basic_types.hpp>
#include <nese/cartridge_mapper/cartridge_mapper.hpp>

namespace nese {

class nrom_cartridge_mapper : public cartridge_mapper
{
public:
    nrom_cartridge_mapper(std::vector<byte_t>&& prg, std::vector<byte_t>&& chr, mirroring mirroring);

    [[nodiscard]] byte_t read(addr_t addr) const override;
    void write(addr_t addr, byte_t value) override;

private:
    addr_t _mask;
};

} // namespace nese