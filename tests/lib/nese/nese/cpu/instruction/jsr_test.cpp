#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/catch_generators.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct jsr_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_absolute(const ExecuteFunctorT& execute)
    {
        SECTION("absolute")
        {
            const auto [addr, addr_to] = GENERATE_ADDR_FOR_ABSOLUTE();

            state.registers.pc = addr;
            state.owned_memory.set_word(addr, addr_to);

            state_mock expected_state = state;
            expected_state.registers.pc = addr_to;

            // high-order bytes push first since the stack grow top->down and the machine is little-endian
            expected_state.owned_memory.set_byte(expected_state.registers.s + stack_offset, (addr + 1) >> 8);
            expected_state.registers.s -= 1;

            expected_state.owned_memory.set_byte(expected_state.registers.s + stack_offset, (addr + 1) & 0xff);
            expected_state.registers.s -= 1;

            execute(state);

            check_state(expected_state);
        }
    }
};

TEST_CASE_METHOD(jsr_fixture, "jsr", "[cpu][instruction]")
{
    test_absolute(execute_jsr<addr_mode::absolute>);
}

} // namespace nese::cpu::instruction
