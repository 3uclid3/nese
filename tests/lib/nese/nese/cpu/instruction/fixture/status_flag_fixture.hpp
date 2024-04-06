#pragma once

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu {

enum class status_flag : u8_t;

namespace instruction {

enum class opcode;

struct status_flag_fixture : execute_fixture
{
    void test_set(opcode opcode, status_flag flag);
    void test_clear(opcode opcode, status_flag flag);
};

} // namespace instruction
} // namespace nese::cpu
