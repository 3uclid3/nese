#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/state.hpp>

namespace nese::cpu::instruction {

bool execute(class execute_context ctx);
bool execute(class execute_context ctx, enum class opcode code);

} // namespace nese::cpu::instruction
