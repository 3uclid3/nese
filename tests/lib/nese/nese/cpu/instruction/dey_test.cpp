#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/decrement_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(decrement_fixture, "dey", "[cpu][instruction]")
{
    test_implied(opcode::dey_implied, behavior_scenarios<register_id::y>);
}

} // namespace nese::cpu::instruction