#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/state_2.hpp>

namespace nese {

namespace memory {
class mapper;
}

namespace cpu::instruction {

class execute_context
{
public:
    constexpr execute_context(state_2& state, memory::mapper& memory) : _state(state), _memory(memory) {}

    [[nodiscard]] constexpr registers& registers() const { return _state.get().registers; }
    [[nodiscard]] constexpr memory::mapper& memory() const { return _memory; }

    constexpr void step_cycle(cpu_cycle_t cycle) const { _state.get().cycle += cycle; }

private:
    ref_wrap<cpu::state_2> _state;
    ref_wrap<memory::mapper> _memory;
};

} // namespace cpu::instruction
} // namespace nese
