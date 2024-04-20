#include <nese/cartridge/cartridge_mapper_factory.hpp>

#include <nese/cartridge/nrom_cartridge_mapper.hpp>
#include <nese/utility/assert.hpp>

namespace nese {

std::unique_ptr<cartridge_mapper> cartridge_mapper_factory::create(cartridge_mapper::id id, std::vector<byte_t>&& prg, std::vector<byte_t>&& chr, cartridge_mapper::mirroring mirroring)
{
    NESE_ASSERT(_creators[id]);

    return (*_creators[id])(std::move(prg), std::move(chr), mirroring);
}

cartridge_mapper_creator_table cartridge_mapper_factory::create_creator_table()
{
    cartridge_mapper_creator_table table{nullptr};

    set_creator<nrom_cartridge_mapper>(table, 0);

    return table;
}

} // namespace nese
