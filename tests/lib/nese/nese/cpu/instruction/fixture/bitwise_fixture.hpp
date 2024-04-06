#pragma once

#include <span>
#include <tuple>

#include <nese/basic_types.hpp>
#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

enum class opcode;

struct bitwise_fixture : execute_fixture
{
    using zero_scenario_t = std::tuple<byte_t, byte_t>;
    using zero_scenarios_t = std::span<const zero_scenario_t>;
    using negative_scenario_t = std::tuple<byte_t, byte_t, byte_t>;
    using negative_scenarios_t = std::span<const negative_scenario_t>;
    using positive_scenario_t = std::tuple<byte_t, byte_t, byte_t>;
    using positive_scenarios_t = std::span<const positive_scenario_t>;

    struct behavior_scenarios
    {
        zero_scenarios_t zero_scenarios;
        negative_scenarios_t negative_scenarios;
        positive_scenarios_t positive_scenarios;
    };

    void test_immediate(opcode opcode, behavior_scenarios behavior_scenarios);
    void test_zero_page(opcode opcode, behavior_scenarios behavior_scenarios);
    void test_zero_page_indexed(opcode opcode, register_id index_register, behavior_scenarios behavior_scenarios);
    void test_absolute(opcode opcode, behavior_scenarios behavior_scenarios);
    void test_absolute_indexed(opcode opcode, register_id index_register, behavior_scenarios behavior_scenarios);
};

} // namespace nese::cpu::instruction
