#pragma once

#include <nese/cycle.hpp>
#include <nese/cpu/registers.hpp>
#include <nese/memory/ram.hpp>

namespace nese::cpu {

struct snapshot
{
    memory::ram ram;
    registers registers;
    cycle_t cycle;
};

} // namespace nese::cpu
