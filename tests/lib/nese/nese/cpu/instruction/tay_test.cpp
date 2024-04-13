#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/transfer_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(transfer_fixture, "tay", "[cpu][instruction]")
{
    test_implied(opcode::tay_implied, behavior_scenarios<register_id::a, register_id::y>);
}

} // namespace nese::cpu::instruction