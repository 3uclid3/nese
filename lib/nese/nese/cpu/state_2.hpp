#pragma once

#include <nese/cpu/registers.hpp>
#include <nese/cycle.hpp>

namespace nese::cpu {

struct state_2
{
    registers registers{};
    cycle_t cycle{0};
};

} // namespace nese::cpu
