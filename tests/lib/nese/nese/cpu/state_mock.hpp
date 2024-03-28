#pragma once

#include <nese/cpu/state.hpp>
#include <nese/memory/mapper.hpp>

namespace nese::cpu {

struct state_mock : state
{
    constexpr state_mock()
        : state{.memory = owned_memory}
    {
    }

    constexpr explicit state_mock(cpu::registers new_registers)
        : state_mock()
    {
        registers = new_registers;
    }

    constexpr state_mock(state_mock&& other)
        : state_mock()
    {
        registers = other.registers;
        owned_memory = other.owned_memory;
        cycle = other.cycle;
    }

    constexpr state_mock(const state_mock& other)
        : state_mock()
    {
        registers = other.registers;
        owned_memory = other.owned_memory;
        cycle = other.cycle;
    }

    constexpr state_mock& operator=(state_mock&& other)
    {
        registers = other.registers;
        owned_memory = other.owned_memory;
        cycle = other.cycle;
        return *this;
    }

    constexpr state_mock& operator=(const state_mock& other)
    {
        registers = other.registers;
        owned_memory = other.owned_memory;
        cycle = other.cycle;
        return *this;
    }

    memory::mapper owned_memory{};
};

constexpr const state& format_as(const state_mock& mock)
{
    return mock;
}

} // namespace nese::cpu
