#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/stack.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(execute_fixture, "jsr", "[cpu][instruction]")
{
    constexpr cycle_t cycle_cost = cpu_cycle_t(6);

    const auto [addr, addr_to] = GENERATE(from_range(absolute_scenarios));

    state().registers.pc = addr;
    memory().set_word(addr, addr_to);

    expected_state().cycle = cycle_cost;
    expected_state().registers.pc = addr_to;

    // high-order bytes push first since the stack grow top->down and the machine is little-endian
    expected_memory().set_byte(expected_state().registers.s + stack_offset, (addr + 1) >> 8);
    expected_state().registers.s -= 1;

    expected_memory().set_byte(expected_state().registers.s + stack_offset, (addr + 1) & 0xff);
    expected_state().registers.s -= 1;

    execute_and_check(opcode::jsr_absolute);
}

} // namespace nese::cpu::instruction
