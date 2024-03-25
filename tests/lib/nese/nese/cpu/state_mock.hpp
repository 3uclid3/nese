#include <nese/cpu/state.hpp>

#include <nese/memory/mapper.hpp>

namespace nese::cpu {

struct state_mock : state
{
    state_mock()
        : state{.memory = owned_memory}
    {
        owned_memory.set_zero();
    }

    explicit state_mock(cpu::registers new_registers)
        : state_mock()
    {
        registers = new_registers;
    }

    memory::mapper owned_memory{};
};

constexpr const state& format_as(const state_mock& mock)
{
    return mock;
}

} // namespace nese::cpu
