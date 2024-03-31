#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(execute_fixture, "nop", "[cpu][instruction]")
{
    constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    const addr_t addr = GENERATE(as<addr_t>(), 0x1234, 0x4321);

    state().registers.pc = addr;

    expected_state().cycle = cycle_cost;

    execute_and_check(opcode::nop_implied);
}

} // namespace nese::cpu::instruction
