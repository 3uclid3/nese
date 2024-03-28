#include <catch2/catch_template_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/catch_generators.hpp>
#include <nese/cpu/instruction/fixture.hpp>
#include <nese/cpu/state_mock.hpp>

namespace nese::cpu::instruction {

struct jmp_fixture : fixture
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

            execute(state);

            check_state(expected_state);
        }
    }
};

TEST_CASE_METHOD(jmp_fixture, "jmp", "[cpu][instruction]")
{
    test_absolute(execute_jmp<addr_mode::absolute>);
}

} // namespace nese::cpu::instruction
