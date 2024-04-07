#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/decrement_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(decrement_fixture, "dex", "[cpu][instruction]")
{
    test_implied(opcode::dex_implied, register_id::x);
}

} // namespace nese::cpu::instruction