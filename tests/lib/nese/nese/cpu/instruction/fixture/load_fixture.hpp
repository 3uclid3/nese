#pragma once

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

enum class opcode;

struct load_fixture : execute_fixture
{
    void test_immediate(opcode opcode, register_id load_register);
    void test_zero_page(opcode opcode, register_id load_register);
    void test_zero_page_indexed(opcode opcode, register_id load_register, register_id index_register);
    void test_absolute(opcode opcode, register_id load_register);
    void test_absolute_indexed(opcode opcode, register_id load_register, register_id index_register);
};

} // namespace nese::cpu::instruction
