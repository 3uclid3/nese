#pragma once

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

enum class opcode;

struct transfer_fixture : execute_fixture
{
    void test_implied(opcode opcode, register_id from_register, register_id to_register, bool affect_flags = true);
};

} // namespace nese::cpu::instruction
