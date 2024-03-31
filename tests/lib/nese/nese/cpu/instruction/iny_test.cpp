#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/increment_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(increment_fixture, "iny", "[cpu][instruction]")
{
    test_inc(opcode::iny_implied , register_id::y);
}

} // namespace nese::cpu::instruction