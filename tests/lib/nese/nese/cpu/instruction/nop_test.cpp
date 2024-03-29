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
            const addr_t addr = GENERATE(addr_t{0x1234}, addr_t{0x4321});

            state.registers.pc = addr;

            state_mock expected_state = state;
            expected_state.cycle = cpu_cycle_t(2);

            execute(state);

            check_state(expected_state);
        }
    }
};

TEST_CASE_METHOD(nop_fixture, "nop", "[cpu][instruction]")
{
    test_implied(execute_nop<addr_mode::implied>);
}

} // namespace nese::cpu::instruction
