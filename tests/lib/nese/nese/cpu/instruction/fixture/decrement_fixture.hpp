#pragma once

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

enum class opcode;

struct decrement_fixture : execute_fixture
{
    void test_implied(opcode opcode, register_id decrement_register);
};

} // namespace nese::cpu::instruction
