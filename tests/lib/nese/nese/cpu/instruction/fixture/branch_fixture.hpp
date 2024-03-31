#pragma once

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu {

enum class status_flag;

namespace instruction {

enum class opcode;

struct branch_fixture : execute_fixture
{
    enum class branch_when
    {
        is_set = 1,
        is_clear = 0
    };

    void test_relative(opcode opcode, status_flag flag, branch_when branch);
};

} // namespace instruction
} // namespace nese::cpu
