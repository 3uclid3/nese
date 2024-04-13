#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/transfer_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(transfer_fixture, "txa", "[cpu][instruction]")
{
    test_implied(opcode::txa_implied, behavior_scenarios<register_id::x, register_id::a>);
}

} // namespace nese::cpu::instruction