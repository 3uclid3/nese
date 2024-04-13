#pragma once

#include <magic_enum.hpp>

#include <nese/cpu/state.hpp>

namespace nese::cpu {

enum class register_id : u8_t
{
    a,
    x,
    y,

    pc,

    s,

    p
};

constexpr auto format_as(register_id id)
{
    return magic_enum::enum_name(id);
}

struct state_mock : state
{
    void set_register(register_id id, byte_t value);
    void set_register(register_id id, word_t value);
};

} // namespace nese::cpu