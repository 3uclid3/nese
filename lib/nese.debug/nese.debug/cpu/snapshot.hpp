#pragma once

#include <nese/cpu/registers.hpp>
#include <nese/cycle.hpp>
#include <nese/memory/mapper.hpp>

namespace nese::cpu {

struct snapshot
{
    memory::mapper memory;
    registers registers;
    cycle_t cycle;
};

} // namespace nese::cpu
