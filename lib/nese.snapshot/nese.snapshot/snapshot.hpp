#pragma once

#include <nese/cpu/state.hpp>
#include <nese/memory/mapper.hpp>

namespace nese {

struct snapshot
{
    memory::mapper memory;
    cpu::state cpu_state;
};

} // namespace nese
