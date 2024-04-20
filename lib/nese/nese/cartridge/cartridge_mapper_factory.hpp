#pragma once

#include <array>

#include <nese/basic_types.hpp>
#include <nese/cartridge/cartridge_mapper.hpp>

namespace nese {

using cartridge_mapper_creator = std::unique_ptr<cartridge_mapper> (*)(std::vector<byte_t>&& prg, std::vector<byte_t>&& chr, cartridge_mapper::mirroring mirroring);
using cartridge_mapper_creator_table = array<cartridge_mapper_creator, std::numeric_limits<byte_t>::max() + 1>;

class cartridge_mapper_factory
{
public:
    static std::unique_ptr<cartridge_mapper> create(cartridge_mapper::id id, std::vector<byte_t>&& prg, std::vector<byte_t>&& chr, cartridge_mapper::mirroring mirroring);

    template<typename MapperT>
    static void set_creator(cartridge_mapper::id id);

private:
    static cartridge_mapper_creator_table create_creator_table();

    template<typename MapperT>
    static void set_creator(cartridge_mapper_creator_table& table, cartridge_mapper::id id);

    inline static cartridge_mapper_creator_table _creators{create_creator_table()};
};

template<typename MapperT>
void cartridge_mapper_factory::set_creator(cartridge_mapper::id id)
{
    set_creator<MapperT>(_creators, id);
}

template<typename MapperT>
void cartridge_mapper_factory::set_creator(cartridge_mapper_creator_table& table, cartridge_mapper::id id)
{
    table[id] = [](std::vector<byte_t>&& prg, std::vector<byte_t>&& chr, cartridge_mapper::mirroring mirroring) -> std::unique_ptr<cartridge_mapper> { return std::make_unique<MapperT>(std::move(prg), std::move(chr), mirroring); };
}

} // namespace nese
