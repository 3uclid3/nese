#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/registers.hpp>
#include <nese/cycle.hpp>

namespace nese::memory {
class mapper;
} // namespace nese::memory

namespace nese::cpu {

struct state
{
    ref_wrap<memory::mapper> memory;
    registers registers{};
    cycle_t cycle{0};
};

} // namespace nese::cpu
