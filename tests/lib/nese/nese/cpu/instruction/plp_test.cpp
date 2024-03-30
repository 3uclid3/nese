#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct plp_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_implied(const ExecuteFunctorT& execute)
    {
        SECTION("implied")
        {
            constexpr cycle_t cycle_cost = cpu_cycle_t(4);

            // Clear all flags
            state.registers.p = 0;

            SECTION("addressing")
            {
                const byte_t s = GENERATE(as<byte_t>(), 0xFD, 0xFE, 0xFF, 0x01, 0x00);
                const byte_t next_s = s + 1;

                INFO(format("s 0x{:02X}", s));

                state.registers.s = s;
                state.owned_memory.set_byte(stack_offset + next_s, 0x00);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.registers.s = next_s;
                expected_state.registers.p = static_cast<u8_t>(status_flag::unused);

                execute(state);

                check_state();
            }

            SECTION("flag set")
            {
                auto flag_set = GENERATE(
                    as<status_flag>(),
                    status_flag::carry,
                    status_flag::zero,
                    status_flag::interrupt,
                    status_flag::decimal,
                    status_flag::unused,
                    status_flag::overflow,
                    status_flag::negative);

                DYNAMIC_SECTION(format("{}", flag_set))
                {
                    state.owned_memory.set_byte(stack_offset + state.registers.s + 1, static_cast<byte_t>(flag_set));

                    expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.s = expected_state.registers.s + 1;
                    expected_state.registers.p = flag_set | status_flag::unused; // unused always set

                    execute(state);

                    check_state();
                }
            }

            SECTION("flag ignored")
            {
                auto flag_ignored = GENERATE(
                    as<status_flag>(),
                    status_flag::break_cmd
                );

                DYNAMIC_SECTION(format("{}", flag_ignored))
                {
                    state.registers.p = static_cast<byte_t>(flag_ignored);
                    state.owned_memory.set_byte(stack_offset + state.registers.s + 1, 0x00);

                    expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.s = expected_state.registers.s + 1;
                    expected_state.registers.p = flag_ignored | status_flag::unused; // unused always set

                    execute(state);

                    check_state();
                }
            }
        }
    }
};

TEST_CASE_METHOD(plp_fixture, "plp", "[cpu][instruction]")
{
    test_implied(execute_plp<addr_mode::implied>);
}

} // namespace nese::cpu::instruction
