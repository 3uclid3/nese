#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/transfer_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(transfer_fixture, "txs", "[cpu][instruction]")
{
    test_implied(opcode::txs_implied, register_id::x, register_id::s, false);
}

} // namespace nese::cpu::instruction