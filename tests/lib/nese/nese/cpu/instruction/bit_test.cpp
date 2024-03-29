#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/catch_generators.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct bit_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_zero_page(const ExecuteFunctorT& execute)
    {
        SECTION("zero_page")
        {
            const auto [pc, val_addr] = GENERATE_ADDR_FOR_ZERO_PAGE();

            state.registers.pc = pc;
            state.owned_memory.set_byte(pc, val_addr);

            const byte_t bit = GENERATE(0x00, 0xff, 0x80, 0x40, 0x01);

            state.owned_memory.set_byte(val_addr, bit);

            state.registers.a = 0xFF;

            expected_state = state;
            expected_state.registers.pc = pc + 1;
            expected_state.registers.set_flag(status_flag::zero, (bit & 0xff) == 0);
            expected_state.registers.set_flag(status_flag::overflow, (bit & 0x40) != 0);
            expected_state.registers.set_flag(status_flag::negative, (bit & 0x80) != 0);

            execute(state);

            check_state();
        }
    }
};

TEST_CASE_METHOD(bit_fixture, "bit", "[cpu][instruction]")
{
    test_zero_page(execute_bit<addr_mode::zero_page>);
}

} // namespace nese::cpu::instruction
