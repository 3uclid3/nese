#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/status_flag.hpp>

namespace nese::cpu::instruction {

enum class opcode;

struct compare_fixture : execute_fixture
{
    static constexpr std::array scenarios = std::to_array<std::tuple<byte_t, byte_t, status_flags>>(
        {
            {0x00, 0x00, status_flag::zero | status_flag::carry},     // Equal, carry and zero set
            {0x01, 0x00, status_flag::carry | 0},                     // A > M, carry set
            {0x00, 0x01, status_flag::negative | 0},                  // A < M, negative set due to borrow, interpreting as "negative" result
            {0x7F, 0x80, status_flag::negative | 0},                  // A < M, negative set, as the subtraction result's MSB is set
            {0x80, 0x7F, status_flag::carry | 0},                     // A > M, carry set
            {0x80, 0x00, status_flag::carry | status_flag::negative}, // A > M, carry and negative set
            {0x00, 0x80, status_flag::negative | 0},                  // A < M, negative set
            {0xFF, 0xFE, status_flag::carry | 0},                     // A > M, carry set
            {0xFE, 0xFF, status_flag::negative | 0},                  // A < M, negative set
            {0x7F, 0x7F, status_flag::zero | status_flag::carry},     // Equal, zero and carry set
            {0x80, 0x80, status_flag::zero | status_flag::carry}      // Equal, zero and carry set
        });

    void test_immediate(opcode opcode, register_id cmp_register);
    void test_zero_page(opcode opcode, register_id cmp_register);
    void test_zero_page_indexed(opcode opcode, register_id cmp_register, register_id index_register);
    void test_absolute(opcode opcode, register_id cmp_register);
    void test_absolute_indexed(opcode opcode, register_id cmp_register, register_id index_register);
};

} // namespace nese::cpu::instruction
