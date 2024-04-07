#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/transfer_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(transfer_fixture, "tya", "[cpu][instruction]")
{
    test_implied(opcode::tya_implied, register_id::y, register_id::a);
}

} // namespace nese::cpu::instruction