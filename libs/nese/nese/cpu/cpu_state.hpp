#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/cpu_registers.hpp>

namespace nese {

struct cpu_state
{
    cpu_registers registers;
    cpu_cycle_t cycle;
};

} // namespace nese
