#pragma once

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

enum class opcode;

struct increment_fixture : execute_fixture
{
    void test_inc(opcode opcode, register_id increment_register);
};

} // namespace nese::cpu::instruction
