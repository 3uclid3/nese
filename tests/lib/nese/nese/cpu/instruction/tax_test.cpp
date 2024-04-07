#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/transfer_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(transfer_fixture, "tax", "[cpu][instruction]")
{
    test_implied(opcode::tax_implied, register_id::a, register_id::x);
}

} // namespace nese::cpu::instruction