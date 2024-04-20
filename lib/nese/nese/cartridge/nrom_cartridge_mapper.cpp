#include <nese/cartridge/nrom_cartridge_mapper.hpp>

#include <nese/utility/assert.hpp>

namespace nese {

nrom_cartridge_mapper::nrom_cartridge_mapper(std::vector<byte_t>&& prg, std::vector<byte_t>&& chr, mirroring mirroring [[maybe_unused]])
    : cartridge_mapper(std::move(prg), std::move(chr))
    , _mask(get_prg().size() > 0x4000 ? 0x7FFF : 0x3FFF)
{
}

byte_t nrom_cartridge_mapper::read(addr_t addr) const
{
    NESE_ASSERT(addr >= 0x8000);

    return get_prg()[addr & _mask];
}

void nrom_cartridge_mapper::write(addr_t addr [[maybe_unused]], byte_t value [[maybe_unused]])
{
}

} // namespace nese