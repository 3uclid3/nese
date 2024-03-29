#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/catch_generators.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct jmp_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_absolute(const ExecuteFunctorT& execute)
    {
        SECTION("absolute")
        {
            constexpr cycle_t cycle_cost = cpu_cycle_t(3);

            const auto [addr, addr_to] = GENERATE_ADDR_FOR_ABSOLUTE();

            state.registers.pc = addr;
            state.owned_memory.set_word(addr, addr_to);

            expected_state = state;
            expected_state.cycle = cycle_cost;
            expected_state.registers.pc = addr_to;

            execute(state);

            check_state();
        }
    }
};

TEST_CASE_METHOD(jmp_fixture, "jmp", "[cpu][instruction]")
{
    test_absolute(execute_jmp<addr_mode::absolute>);
}

} // namespace nese::cpu::instruction
