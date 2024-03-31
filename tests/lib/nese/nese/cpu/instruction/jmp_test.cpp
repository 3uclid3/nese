#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(execute_fixture, "jmp", "[cpu][instruction]")
{
    constexpr cycle_t cycle_cost = cpu_cycle_t(3);

    const auto [addr, addr_to] = GENERATE(from_range(absolute_scenarios));

    state().registers.pc = addr;
    memory().set_word(addr, addr_to);

    expected_state().cycle = cycle_cost;
    expected_state().registers.pc = addr_to;

    execute_and_check(opcode::jmp_absolute);
}

} // namespace nese::cpu::instruction
