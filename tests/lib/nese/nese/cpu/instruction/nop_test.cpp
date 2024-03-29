#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct nop_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_implied(const ExecuteFunctorT& execute)
    {
        SECTION("implied")
        {
            constexpr cycle_t cycle_cost = cpu_cycle_t(2);

            const addr_t addr = GENERATE(as<addr_t>(), 0x1234, 0x4321);

            state.registers.pc = addr;

            expected_state = state;
            expected_state.cycle = cycle_cost;

            execute(state);

            check_state();
        }
    }
};

TEST_CASE_METHOD(nop_fixture, "nop", "[cpu][instruction]")
{
    test_implied(execute_nop<addr_mode::implied>);
}

} // namespace nese::cpu::instruction
