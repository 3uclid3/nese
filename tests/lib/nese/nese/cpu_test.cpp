#include <catch2/catch_test_macros.hpp>

#include <nese/cpu.hpp>
#include <nese/cpu_fixture.hpp>

namespace nese {

struct cpu_branch_fixture : cpu_fixture
{
    static constexpr cpu_cycle_t no_branching_cycle_cost = cpu_cycle_t(2);
    static constexpr cpu_cycle_t no_page_crossing_cycle_cost = cpu_cycle_t(3);
    static constexpr cpu_cycle_t page_crossing_cycle_cost = cpu_cycle_t(4);
};

struct cpu_branch_if_set_fixture : cpu_branch_fixture
{
    // clang-format off
    template<cpu_status StatusT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
         // no page crossing
        {
            .initial = {op::set_register_status<StatusT>(), set_pc(0x0000), set_memory(0x0000, 0x10)},
            .expected = {set_pc(0x0011)},
            .description = "within the first page",
            .cycle_cost = no_page_crossing_cycle_cost
        },
        {
            .initial = {set_pc(0x0000), set_memory(0x0000, 0x10)},
            .expected = {set_pc(0x0001)},
            .description = "no branching within the first page",
            .cycle_cost = no_branching_cycle_cost
        },
        {
            .initial = {op::set_register_status<StatusT>(), set_pc(0x0080), set_memory(0x0080, 0x7E)},
            .expected = {set_pc(0x0081 + 0x7E)},
            .description = "near the middle of the first page, large offset but no crossing",
            .cycle_cost = no_page_crossing_cycle_cost
        },
        {
            .initial = {set_pc(0x0080), set_memory(0x0080, 0x7E)},
            .expected = {set_pc(0x0081)},
            .description = "no branching near the middle of the first page, large offset but no crossing",
            .cycle_cost = no_branching_cycle_cost
        },

         // page crossing
        {
            .initial = {op::set_register_status<StatusT>(), set_pc(0x00F0), set_memory(0x00F0, 0x0F)},
            .expected = {set_pc(0x0100)},
            .description = "crosses from 0x00xx to 0x01xx page",
            .cycle_cost = page_crossing_cycle_cost
        },
        {
            .initial = {set_pc(0x00F0), set_memory(0x00F0, 0x0F)},
            .expected = {set_pc(0x00F1)},
            .description = "no branching crosses from 0x00xx to 0x01xx page",
            .cycle_cost = no_branching_cycle_cost
        },
        {
            .initial = {op::set_register_status<StatusT>(), set_pc(0x01FD), set_memory(0x01FD, 0x02)},
            .expected = {set_pc(0x0200)},
            .description = "crosses from 0x01xx to 0x02xx page",
            .cycle_cost = page_crossing_cycle_cost
        },
        {
            .initial = {set_pc(0x01FD), set_memory(0x01FD, 0x02)},
            .expected = {set_pc(0x01FE)},
            .description = "no branching crosses from 0x01xx to 0x02xx page",
            .cycle_cost = no_branching_cycle_cost
        },
        {
            .initial = {op::set_register_status<StatusT>(), set_pc(0x0FFE), set_memory(0x0FFE, 0x01)},
            .expected = {set_pc(0x1000)},
            .description = "crosses from 0x0Fxx to 0x10xx page",
            .cycle_cost = page_crossing_cycle_cost
        }
    });
    // clang-format on
};

struct cpu_branch_if_clear_fixture : cpu_branch_fixture
{
    // clang-format off
    template<cpu_status StatusT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // no page crossing
        {
            .initial = {set_pc(0x0000), set_memory(0x0000, 0x10)},
            .expected = {set_pc(0x0011)},
            .description = "within the first page",
            .cycle_cost = no_page_crossing_cycle_cost
        },
        {
            .initial = {op::set_register_status<StatusT>(), set_pc(0x0000), set_memory(0x0000, 0x10)},
            .expected = {set_pc(0x0001)},
            .description = "no branching within the first page",
            .cycle_cost = no_branching_cycle_cost
        },
        {
            .initial = {set_pc(0x0080), set_memory(0x0080, 0x7E)},
            .expected = {set_pc(0x0081 + 0x7E)},
            .description = "near the middle of the first page, large offset but no crossing",
            .cycle_cost = no_page_crossing_cycle_cost
        },
        {
            .initial = {op::set_register_status<StatusT>(), set_pc(0x0080), set_memory(0x0080, 0x7E)},
            .expected = {set_pc(0x0081)},
            .description = "no branching near the middle of the first page, large offset but no crossing",
            .cycle_cost = no_branching_cycle_cost
        },

         // page crossing
        {
            .initial = {set_pc(0x00F0), set_memory(0x00F0, 0x0F)},
            .expected = {set_pc(0x0100)},
            .description = "crosses from 0x00xx to 0x01xx page",
            .cycle_cost = page_crossing_cycle_cost
        },
        {
            .initial = {op::set_register_status<StatusT>(), set_pc(0x00F0), set_memory(0x00F0, 0x0F)},
            .expected = {set_pc(0x00F1)},
            .description = "no branching crosses from 0x00xx to 0x01xx page",
            .cycle_cost = no_branching_cycle_cost
        },
        {
            .initial = {set_pc(0x01FD), set_memory(0x01FD, 0x02)},
            .expected = {set_pc(0x0200)},
            .description = "crosses from 0x01xx to 0x02xx page",
            .cycle_cost = page_crossing_cycle_cost
        },
        {
            .initial = {op::set_register_status<StatusT>(), set_pc(0x01FD), set_memory(0x01FD, 0x02)},
            .expected = {set_pc(0x01FE)},
            .description = "no branching crosses from 0x01xx to 0x02xx page",
            .cycle_cost = no_branching_cycle_cost
        },
        {
            .initial = {set_pc(0x0FFE), set_memory(0x0FFE, 0x01)},
            .expected = {set_pc(0x1000)},
            .description = "crosses from 0x0Fxx to 0x10xx page",
            .cycle_cost = page_crossing_cycle_cost
        }
    });
    // clang-format on
};

struct cpu_clear_status_fixture : cpu_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    // clang-format off
    template<cpu_status StatusFlagT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {op::set_register_status<StatusFlagT>()},
            .expected = {op::clear_register_status<StatusFlagT>()},
            .description = "initially set",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::clear_register_status<StatusFlagT>()},
            .expected = {op::clear_register_status<StatusFlagT>()},
            .description = "initially clear",
            .cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

struct cpu_compare_fixture : cpu_fixture
{
    // clang-format off
    template<cpu_register_id RegisterT>
    inline static const scenario addr_mode_scenario{
        .initial = {op::set_register<RegisterT>(0), set_operand(0)},
        .expected = {set_status_carry(), set_status_zero()}
    };

    template<cpu_register_id RegisterT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {op::set_register<RegisterT>(0x01), set_operand(0x00)},
            .expected = {set_status_carry()},
        },
        {
            .initial = {op::set_register<RegisterT>(0x00), set_operand(0x01)},
            .expected = {set_status_negative()},
        },
        {
            .initial = {op::set_register<RegisterT>(0x7F), set_operand(0x80)},
            .expected = {set_status_negative()},
        },
        {
            .initial = {op::set_register<RegisterT>(0x80), set_operand(0x7F)},
            .expected = {set_status_carry()},
        },
        {
            .initial = {op::set_register<RegisterT>(0x80), set_operand(0x00)},
            .expected = {set_status_carry(), set_status_negative()},
        },
        {
            .initial = {op::set_register<RegisterT>(0x00), set_operand(0x80)},
            .expected = {set_status_negative()},
        },
        {
            .initial = {op::set_register<RegisterT>(0xFF), set_operand(0xFE)},
            .expected = {set_status_carry()},
        },
        {
            .initial = {op::set_register<RegisterT>(0xFE), set_operand(0xFF)},
            .expected = {set_status_negative()},
        },
        {
            .initial = {op::set_register<RegisterT>(0x7F), set_operand(0x7F)},
            .expected = {set_status_carry(), set_status_zero()},
        },
        {
            .initial = {op::set_register<RegisterT>(0x80), set_operand(0x80)},
            .expected = {set_status_carry(), set_status_zero()},
        },

    });
    // clang-format on
};

struct cpu_decrement_fixture : cpu_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    // clang-format off
    template<cpu_register_id RegisterT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        scenario{
            .initial = {op::set_register<RegisterT>(0x00), set_status_zero()},
            .expected = {op::set_register<RegisterT>(0xFF), clear_status_zero(), set_status_negative()},
            .description = "0x00 results in overflow to 0xFF",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x80), set_status_negative()},
            .expected = {op::set_register<RegisterT>(0x7F), clear_status_negative()},
            .description = "0x80 results in overflow to 0x7F",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x01)},
            .expected = {op::set_register<RegisterT>(0x00), set_status_zero()},
            .description = "zero",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x81), set_status_negative()},
            .expected = {op::set_register<RegisterT>(0x80)},
            .description = "smallest negative value minus one",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0xC1), set_status_negative()},
            .expected = {op::set_register<RegisterT>(0xC0)},
            .description = "mid negative value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0xFF), set_status_negative()},
            .expected = {op::set_register<RegisterT>(0xFE)},
            .description = "largest negative value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x02)},
            .expected = {op::set_register<RegisterT>(0x01)},
            .description = "smallest positive value minus one",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x41)},
            .expected = {op::set_register<RegisterT>(0x40)},
            .description = "mid positive value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x7F)},
            .expected = {op::set_register<RegisterT>(0x7E)},
            .description = "largest positive value",
            .cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

struct cpu_increment_fixture : cpu_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    // clang-format off
    template<cpu_register_id RegisterT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {op::set_register<RegisterT>(0xFF), set_status_negative()},
            .expected = {op::set_register<RegisterT>(0x00), set_status_zero(), clear_status_negative()},
            .description = "0xFF results in overflow to 0x00",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x7F)},
            .expected = {op::set_register<RegisterT>(0x80), set_status_negative()},
            .description = "0x7F results in overflow to 0x80",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x00), set_status_zero()},
            .expected = {op::set_register<RegisterT>(0x01), clear_status_zero()},
            .description = "zero",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x80), set_status_negative()},
            .expected = {op::set_register<RegisterT>(0x81)},
            .description = "smallest negative value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0xC0), set_status_negative()},
            .expected = {op::set_register<RegisterT>(0xC1)},
            .description = "mid negative value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0xFE), set_status_negative()},
            .expected = {op::set_register<RegisterT>(0xFF)},
            .description = "largest negative value minus one",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x01)},
            .expected = {op::set_register<RegisterT>(0x02)},
            .description = "smallest positive value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x40)},
            .expected = {op::set_register<RegisterT>(0x41)},
            .description = "mid positive value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<RegisterT>(0x7E)},
            .expected = {op::set_register<RegisterT>(0x7F)},
            .description = "largest positive value minus one",
            .cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

struct cpu_load_fixture : cpu_fixture
{
    // clang-format off
    template<cpu_register_id RegisterT>
    inline static const scenario addr_mode_scenario{
        .initial = {set_operand(1)},
        .expected = {op::set_register<RegisterT>(1)}
    };

    template<cpu_register_id RegisterT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_operand(0)},
            .expected = {op::set_register<RegisterT>(0), set_status_zero()},
            .description = "zero value"
        },
        {
            .initial = {set_operand(0x01)},
            .expected = {op::set_register<RegisterT>(0x01)},
            .description = "smallest positive value"
        },
        {
            .initial = {set_operand(0x40)},
            .expected = {op::set_register<RegisterT>(0x40)},
            .description = "mid positive value"
        },
        {
            .initial = {set_operand(0x7F)},
            .expected = {op::set_register<RegisterT>(0x7F)},
            .description = "largest positive value"
        },
        {
            .initial = {set_operand(0xFF)},
            .expected = {op::set_register<RegisterT>(0xFF), set_status_negative()},
            .description = "smallest negative value"
        },
        {
            .initial = {set_operand(0xC0)},
            .expected = {op::set_register<RegisterT>(0xC0), set_status_negative()},
            .description = "mid negative value"
        },
        {
            .initial = {set_operand(0x80)},
            .expected = {op::set_register<RegisterT>(0x80), set_status_negative()},
            .description = "largest negative value"
        }
    });
    // clang-format on
};

struct cpu_set_status_fixture : cpu_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    // clang-format off
    template<cpu_status StatusFlagT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {op::set_register_status<StatusFlagT>()},
            .expected = {op::set_register_status<StatusFlagT>()},
            .description = "initially set",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::clear_register_status<StatusFlagT>()},
            .expected = {op::set_register_status<StatusFlagT>()},
            .description = "initially clear",
            .cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

struct cpu_store_fixture : cpu_fixture
{
    // clang-format off
    template<cpu_register_id RegisterT>
    inline static const scenario addr_mode_scenario{
        .initial = {op::set_register<RegisterT>(1)},
        .expected = {set_operand(1)}
    };

    template<cpu_register_id RegisterT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {op::set_register<RegisterT>(0x00)},
            .expected = {set_operand(0x00)},
            .description = "zero value"
        },
        {
            .initial = {op::set_register<RegisterT>(0x80)},
            .expected = {set_operand(0x80)},
            .description = "mid value"
        },
        {
            .initial = {op::set_register<RegisterT>(0xFF)},
            .expected = {set_operand(0xFF)},
            .description = "max value"
        },
    });
    // clang-format on
};

struct cpu_transfer_fixture : cpu_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    // clang-format off
    template<cpu_register_id FromRegisterT, cpu_register_id ToRegisterT, bool AffectFlags = true>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {op::set_register<FromRegisterT>(0x00)},
            .expected = {op::set_register<ToRegisterT>(0x00), conditional(set_status_zero(), AffectFlags)},
            .description = "zero value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<FromRegisterT>(0x01)},
            .expected = {op::set_register<ToRegisterT>(0x01)},
            .description = "smallest positive value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<FromRegisterT>(0x40)},
            .expected = {op::set_register<ToRegisterT>(0x40)},
            .description = "mid positive value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<FromRegisterT>(0x7F)},
            .expected = {op::set_register<ToRegisterT>(0x7F)},
            .description = "largest positive value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<FromRegisterT>(0xFF)},
            .expected = {op::set_register<ToRegisterT>(0xFF), conditional(set_status_negative(), AffectFlags)},
            .description = "smallest negative value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<FromRegisterT>(0xC0)},
            .expected = {op::set_register<ToRegisterT>(0xC0), conditional(set_status_negative(), AffectFlags)},
            .description = "mid negative value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {op::set_register<FromRegisterT>(0x80)},
            .expected = {op::set_register<ToRegisterT>(0x80), conditional(set_status_negative(), AffectFlags)},
            .description = "largest negative value",
            .cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

TEST_CASE_METHOD(cpu_fixture, "adc", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_a(0x00), set_operand(0x00)},
        .expected = {set_status_zero()}
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        // carry flag initially clear
        {
            .initial = {set_a(0x01), set_operand(0x01)},
            .expected = {set_a(0x02)}
        },
        {
            .initial = {set_a(0x7F), set_operand(0x01)},
            .expected = {set_a(0x80), set_status_negative(), set_status_overflow()}
        },
        {
            .initial = {set_a(0xFF), set_operand(0x01)},
            .expected = {set_a(0x00), set_status_carry(), set_status_zero()}
        },
        {
            .initial = {set_a(0x80), set_operand(0x7F)},
            .expected = {set_a(0xFF), set_status_negative()}
        },
        
        // carry flag initially set
        {
            .initial = {set_a(0x00), set_operand(0x00), set_status_carry()},
            .expected = {set_a(0x01), clear_status_carry()}
        },
        {
            .initial = {set_a(0x01), set_operand(0x01), set_status_carry()},
            .expected = {set_a(0x03), clear_status_carry()}
        },
        {
            .initial = {set_a(0xFF), set_operand(0x00), set_status_carry()},
            .expected = {set_a(0x00), set_status_zero()}
        },
        {
            .initial = {set_a(0x7F), set_operand(0x00), set_status_carry()},
            .expected = {set_a(0x80), clear_status_carry(), set_status_negative(), set_status_overflow()}
        },
        {
            .initial = {set_a(0x80), set_operand(0x7F), set_status_carry()},
            .expected = {set_a(0x00), set_status_zero()}
        },
        {
            .initial = {set_a(0x7F), set_operand(0x01), set_status_carry()},
            .expected = {set_a(0x81), clear_status_carry(), set_status_negative(), set_status_overflow()}
        }
    });
    // clang-format on

    test_immediate(cpu_opcode::adc_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(cpu_opcode::adc_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::adc_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::adc_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::adc_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::adc_absolute_y, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::adc_indexed_indirect, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(cpu_opcode::adc_indirect_indexed, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "and", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_a(0), set_operand(0)},
        .expected = {set_a(0), set_status_zero()}
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        // zero
        {
            .initial = {set_a(0x00), set_operand(0xFF)},
            .expected = {set_a(0x00), set_status_zero()},
        },
        {
            .initial = {set_a(0xFF), set_operand(0x00)},
            .expected = {set_a(0x00), set_status_zero()},
        },
        {
            .initial = {set_a(0x0F), set_operand(0xF0)},
            .expected = {set_a(0x00), set_status_zero()},
        },
        
        // negative
        {
            .initial = {set_a(0x80), set_operand(0xFF)},
            .expected = {set_a(0x80), set_status_negative()},
        },
        {
            .initial = {set_a(0xFF), set_operand(0x80)},
            .expected = {set_a(0x80), set_status_negative()},
        },
        {
            .initial = {set_a(0xC0), set_operand(0xF0)},
            .expected = {set_a(0xC0), set_status_negative()},
        },
        {
            .initial = {set_a(0xF0), set_operand(0xC0)},
            .expected = {set_a(0xC0), set_status_negative()},
        },
        {
            .initial = {set_a(0x80), set_operand(0x80)},
            .expected = {set_a(0x80), set_status_negative()},
        },
        {
            .initial = {set_a(0xFF), set_operand(0xC0)},
            .expected = {set_a(0xC0), set_status_negative()},
        },
        
        // positive
        {
            .initial = {set_a(0x7F), set_operand(0xFF)},
            .expected = {set_a(0x7F)},
        },
        {
            .initial = {set_a(0x3F), set_operand(0x7F)},
            .expected = {set_a(0x3F)},
        },
        {
            .initial = {set_a(0x1F), set_operand(0x2F)},
            .expected = {set_a(0x0F)},
        },
        {
            .initial = {set_a(0x0F), set_operand(0x8F)},
            .expected = {set_a(0x0F)},
        },
        {
            .initial = {set_a(0x7F), set_operand(0x3F)},
            .expected = {set_a(0x3F)},
        },
        {
            .initial = {set_a(0x2F), set_operand(0x1F)},
            .expected = {set_a(0x0F)},
        },
        {
            .initial = {set_a(0x01), set_operand(0x01)},
            .expected = {set_a(0x01)},
        }
    });
    // clang-format on

    test_immediate(cpu_opcode::and_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(cpu_opcode::and_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::and_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::and_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::and_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::and_absolute_y, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::and_indexed_indirect, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(cpu_opcode::and_indirect_indexed, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "asl", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_operand(0x00)},
        .expected = {set_operand(0x00), set_status_zero()}
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        // flag carry = (original value & 0x80)
        {
            .initial= {set_operand(0b10000000)},
            .expected = {set_operand(0b00000000), set_status_carry(), set_status_zero()}
        },
        {
            .initial= {set_operand(0b10000001)},
            .expected = {set_operand(0b00000010), set_status_carry()}
        },
        
        // flag negative
        {
            .initial= {set_operand(0b01000000)},
            .expected = {set_operand(0b10000000), set_status_negative()}
        },
        {
            .initial= {set_operand(0b01001000)},
            .expected = {set_operand(0b10010000), set_status_negative()}
        },
        
        // no flag
        {
            .initial= {set_operand(0b00000100)},
            .expected = {set_operand(0b00001000)}
        },
    });
    // clang-format on

    test_acculumator(cpu_opcode::asl_accumulator, behavior_scenarios);
    test_zero_page(cpu_opcode::asl_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::asl_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::asl_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::asl_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_branch_if_clear_fixture, "bcc", "[cpu][instruction]")
{
    test_relative(cpu_opcode::bcc_relative, behavior_scenarios<cpu_status::carry>);
}

TEST_CASE_METHOD(cpu_branch_if_set_fixture, "bcs", "[cpu][instruction]")
{
    test_relative(cpu_opcode::bcs_relative, behavior_scenarios<cpu_status::carry>);
}

TEST_CASE_METHOD(cpu_branch_if_set_fixture, "beq", "[cpu][instruction]")
{
    test_relative(cpu_opcode::beq_relative, behavior_scenarios<cpu_status::zero>);
}

TEST_CASE_METHOD(cpu_fixture, "bit", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_a(0xFF), set_operand(0x01)},
        .expected = {}
    };
    
    static const std::array behavior_scenarios = std::to_array<scenario>({
        // set zero
        {
            .initial = {set_a(0xFF), set_operand(0x00)},
            .expected = {set_status_zero()},
        },
        {
            .initial = {set_a(0xF0), set_operand(0x0F)},
            .expected = {set_status_zero()},
        },
        {
            .initial = {set_a(0x40), set_operand(0x3F)},
            .expected = {set_status_zero()},
        },
        {
            .initial = {set_a(0x20), set_operand(0x1F)},
            .expected = {set_status_zero()},
        },

        // not set zero
        {
            .initial = {set_a(0xBF), set_operand(0x3F)},
            .expected = {clear_status_zero()},
        },
        {
            .initial = {set_a(0x9F), set_operand(0x2F)},
            .expected = {clear_status_zero()},
        },
        
        // set overflow
        {
            .initial = {set_a(0xFF), set_operand(0x40)},
            .expected = {set_status_overflow()},
        },
        {
            .initial = {set_a(0xFF), set_operand(0x41)},
            .expected = {set_status_overflow()},
        },
        {
            .initial = {set_a(0x7F), set_operand(0x40)},
            .expected = {set_status_overflow()},
        },
        {
            .initial = {set_a(0xBF), set_operand(0x42)},
            .expected = {set_status_overflow()},
        },
        
        // not set overflow
        {
            .initial = {set_a(0x01), set_operand(0x01)},
            .expected = {clear_status_overflow()},
        },
        {
            .initial = {set_a(0x03), set_operand(0x02)},
            .expected = {clear_status_overflow()},
        },
        {
            .initial = {set_a(0x0C), set_operand(0x08)},
            .expected = {clear_status_overflow()},
        },
        {
            .initial = {set_a(0x10), set_operand(0x10)},
            .expected = {clear_status_overflow()},
        },
        
        // set negative
        {
            .initial = {set_a(0xFF), set_operand(0x80)},
            .expected = {set_status_negative()},
        },
        {
            .initial = {set_a(0xFF), set_operand(0x81)},
            .expected = {set_status_negative()},
        },
        {
            .initial = {set_a(0x80), set_operand(0x80)},
            .expected = {set_status_negative()},
        },
        {
            .initial = {set_a(0xBF), set_operand(0x82)},
            .expected = {set_status_negative()},
        },
        
        // not set negative
        {
            .initial = {set_a(0xFF), set_operand(0x3F)},
            .expected = {clear_status_negative()},
        },
        {
            .initial = {set_a(0xEF), set_operand(0x20)},
            .expected = {clear_status_negative()},
        },
        {
            .initial = {set_a(0xF7), set_operand(0x1F)},
            .expected = {clear_status_negative()},
        }
    });
    // clang-format on

    test_zero_page(cpu_opcode::bit_zero_page, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::bit_absolute, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_branch_if_set_fixture, "bmi", "[cpu][instruction]")
{
    test_relative(cpu_opcode::bmi_relative, behavior_scenarios<cpu_status::negative>);
}

TEST_CASE_METHOD(cpu_branch_if_clear_fixture, "bne", "[cpu][instruction]")
{
    test_relative(cpu_opcode::bne_relative, behavior_scenarios<cpu_status::zero>);
}

TEST_CASE_METHOD(cpu_branch_if_clear_fixture, "bpl", "[cpu][instruction]")
{
    test_relative(cpu_opcode::bpl_relative, behavior_scenarios<cpu_status::negative>);
}

TEST_CASE_METHOD(cpu_branch_if_clear_fixture, "bvc", "[cpu][instruction]")
{
    test_relative(cpu_opcode::bvc_relative, behavior_scenarios<cpu_status::overflow>);
}

TEST_CASE_METHOD(cpu_branch_if_set_fixture, "bvs", "[cpu][instruction]")
{
    test_relative(cpu_opcode::bvs_relative, behavior_scenarios<cpu_status::overflow>);
}

TEST_CASE_METHOD(cpu_clear_status_fixture, "clc", "[cpu][instruction]")
{
    test_implied(cpu_opcode::clc_implied, behavior_scenarios<cpu_status::carry>);
}

TEST_CASE_METHOD(cpu_clear_status_fixture, "cld", "[cpu][instruction]")
{
    test_implied(cpu_opcode::cld_implied, behavior_scenarios<cpu_status::decimal>);
}

TEST_CASE_METHOD(cpu_clear_status_fixture, "cli", "[cpu][instruction]")
{
    test_implied(cpu_opcode::cli_implied, behavior_scenarios<cpu_status::interrupt>);
}

TEST_CASE_METHOD(cpu_clear_status_fixture, "clv", "[cpu][instruction]")
{
    test_implied(cpu_opcode::clv_implied, behavior_scenarios<cpu_status::overflow>);
}

TEST_CASE_METHOD(cpu_compare_fixture, "cmp", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = cpu_compare_fixture::addr_mode_scenario<cpu_register_id::a>;
    const auto& behavior_scenarios = cpu_compare_fixture::behavior_scenarios<cpu_register_id::a>;

    test_immediate(cpu_opcode::cmp_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(cpu_opcode::cmp_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::cmp_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::cmp_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::cmp_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::cmp_absolute_y, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::cmp_indexed_indirect, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(cpu_opcode::cmp_indirect_indexed, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_compare_fixture, "cpx", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = cpu_compare_fixture::addr_mode_scenario<cpu_register_id::x>;
    const auto& behavior_scenarios = cpu_compare_fixture::behavior_scenarios<cpu_register_id::x>;

    test_immediate(cpu_opcode::cpx_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(cpu_opcode::cpx_zero_page, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::cpx_absolute, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_compare_fixture, "cpy", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = cpu_compare_fixture::addr_mode_scenario<cpu_register_id::y>;
    const auto& behavior_scenarios = cpu_compare_fixture::behavior_scenarios<cpu_register_id::y>;

    test_immediate(cpu_opcode::cpy_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(cpu_opcode::cpy_zero_page, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::cpy_absolute, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "dec", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_operand(0x02)},
        .expected = {set_operand(0x01)}
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_operand(0x00), set_status_zero()},
            .expected = {set_operand(0xFF), clear_status_zero(), set_status_negative()},
            .description = "0x00 results in overflow to 0xFF",
        },
        {
            .initial = {set_operand(0x80), set_status_negative()},
            .expected = {set_operand(0x7F), clear_status_negative()},
            .description = "0x80 results in overflow to 0x7F",
        },
        {
            .initial = {set_operand(0x01)},
            .expected = {set_operand(0x00), set_status_zero()},
            .description = "zero",
        },
        {
            .initial = {set_operand(0x81), set_status_negative()},
            .expected = {set_operand(0x80)},
            .description = "smallest negative value minus one",
        },
        {
            .initial = {set_operand(0xC1), set_status_negative()},
            .expected = {set_operand(0xC0)},
            .description = "mid negative value",
        },
        {
            .initial = {set_operand(0xFF), set_status_negative()},
            .expected = {set_operand(0xFE)},
            .description = "largest negative value",
        },
        {
            .initial = {set_operand(0x02)},
            .expected = {set_operand(0x01)},
            .description = "smallest positive value minus one",
        },
        {
            .initial = {set_operand(0x41)},
            .expected = {set_operand(0x40)},
            .description = "mid positive value",
        },
        {
            .initial = {set_operand(0x7F)},
            .expected = {set_operand(0x7E)},
            .description = "largest positive value",
        }
    });
    // clang-format on

    test_zero_page(cpu_opcode::dec_zero_page, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(5));
    test_zero_page_indexed(cpu_opcode::dec_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(6));
    test_absolute(cpu_opcode::dec_absolute, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(6));
    test_absolute_indexed(cpu_opcode::dec_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(7));
}

TEST_CASE_METHOD(cpu_decrement_fixture, "dex", "[cpu][instruction]")
{
    test_implied(cpu_opcode::dex_implied, behavior_scenarios<cpu_register_id::x>);
}

TEST_CASE_METHOD(cpu_decrement_fixture, "dey", "[cpu][instruction]")
{
    test_implied(cpu_opcode::dey_implied, behavior_scenarios<cpu_register_id::y>);
}

TEST_CASE_METHOD(cpu_fixture, "eor", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_a(0), set_operand(0)},
        .expected = {set_a(0), set_status_zero()}
    };
    
    static const std::array behavior_scenarios = std::to_array<scenario>({
        // zero
        {
            .initial = {set_a(0x00), set_operand(0x00)},
            .expected = {set_a(0x00), set_status_zero()},
        },
        {
            .initial = {set_a(0x80), set_operand(0x80)},
            .expected = {set_a(0x00), set_status_zero()},
        },
        
        // negative
        {
            .initial = {set_a(0x80), set_operand(0x00)},
            .expected = {set_a(0x80), set_status_negative()},
        },
        {
            .initial = {set_a(0x00), set_operand(0x80)},
            .expected = {set_a(0x80), set_status_negative()},
        },
        {
            .initial = {set_a(0xFF), set_operand(0x7F)},
            .expected = {set_a(0x80), set_status_negative()},
        },
        {
            .initial = {set_a(0x7F), set_operand(0xFF)},
            .expected = {set_a(0x80), set_status_negative()},
        },
        {
            .initial = {set_a(0xC0), set_operand(0x40)},
            .expected = {set_a(0x80), set_status_negative()},
        },
        {
            .initial = {set_a(0x40), set_operand(0xC0)},
            .expected = {set_a(0x80), set_status_negative()},
        },
        
        // positive
        {
            .initial = {set_a(0x01), set_operand(0x02)},
            .expected = {set_a(0x03)},
        },
        {
            .initial = {set_a(0x0F), set_operand(0x10)},
            .expected = {set_a(0x1F)},
        },
        {
            .initial = {set_a(0x20), set_operand(0x03)},
            .expected = {set_a(0x23)},
        },
        {
            .initial = {set_a(0x4C), set_operand(0x33)},
            .expected = {set_a(0x7F)},
        },
        {
            .initial = {set_a(0x22), set_operand(0x11)},
            .expected = {set_a(0x33)},
        },
    });
    // clang-format on

    test_immediate(cpu_opcode::eor_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(cpu_opcode::eor_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::eor_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::eor_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::eor_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::eor_absolute_y, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::eor_indexed_indirect, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(cpu_opcode::eor_indirect_indexed, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "inc", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_operand(0x01)},
        .expected = {set_operand(0x02)}
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_operand(0xFF), set_status_negative()},
            .expected = {set_operand(0x00), set_status_zero(), clear_status_negative()},
            .description = "0xFF results in overflow to 0x00"
        },
        {
            .initial = {set_operand(0x7F)},
            .expected = {set_operand(0x80), set_status_negative()},
            .description = "0x7F results in overflow to 0x80"
        },
        {
            .initial = {set_operand(0x00), set_status_zero()},
            .expected = {set_operand(0x01), clear_status_zero()},
            .description = "zero"
        },
        {
            .initial = {set_operand(0x80), set_status_negative()},
            .expected = {set_operand(0x81)},
            .description = "smallest negative value"
        },
        {
            .initial = {set_operand(0xC0), set_status_negative()},
            .expected = {set_operand(0xC1)},
            .description = "mid negative value"
        },
        {
            .initial = {set_operand(0xFE), set_status_negative()},
            .expected = {set_operand(0xFF)},
            .description = "largest negative value minus one"
        },
        {
            .initial = {set_operand(0x01)},
            .expected = {set_operand(0x02)},
            .description = "smallest positive value"
        },
        {
            .initial = {set_operand(0x40)},
            .expected = {set_operand(0x41)},
            .description = "mid positive value"
        },
        {
            .initial = {set_operand(0x7E)},
            .expected = {set_operand(0x7F)},
            .description = "largest positive value minus one"
        }
    });
    // clang-format on

    test_zero_page(cpu_opcode::inc_zero_page, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(5));
    test_zero_page_indexed(cpu_opcode::inc_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(6));
    test_absolute(cpu_opcode::inc_absolute, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(6));
    test_absolute_indexed(cpu_opcode::inc_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(7));
}

TEST_CASE_METHOD(cpu_increment_fixture, "inx", "[cpu][instruction]")
{
    test_implied(cpu_opcode::inx_implied, behavior_scenarios<cpu_register_id::x>);
}

TEST_CASE_METHOD(cpu_increment_fixture, "iny", "[cpu][instruction]")
{
    test_implied(cpu_opcode::iny_implied, behavior_scenarios<cpu_register_id::y>);
}

TEST_CASE_METHOD(cpu_fixture, "jmp", "[cpu][instruction]")
{
    static constexpr cpu_cycle_t absolute_cycle_cost = cpu_cycle_t(3);

    // clang-format off
    static const std::array absolute_behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_pc(0x0200), set_memoryw(0x0200, 0x0100)},
            .expected = {set_pc(0x0100)}
        },
        {
            .initial = {set_pc(0x0200), set_memoryw(0x0200, 0x8000)},
            .expected = {set_pc(0x8000)}
        },
        
        {
            .initial = {set_pc(0x01FF), set_memoryw(0x01FF, 0x0000)},
            .expected = {set_pc(0x0000)}
        },
        {
            .initial = {set_pc(0x0200), set_memoryw(0x0200, 0xFFFF)},
            .expected = {set_pc(0xFFFF)}
        },
        {
            .initial = {set_pc(0xF000), set_memoryw(0xF000, 0x4000)},
            .expected = {set_pc(0x4000)}
        },

        {
            .initial = {set_pc(0x0000), set_memoryw(0x0000, 0x0200)},
            .expected = {set_pc(0x0200)}
        },
        {
            .initial = {set_pc(0x8000), set_memoryw(0x8000, 0x0300)},
            .expected = {set_pc(0x0300)}
        },
        {
            .initial = {set_pc(0xFFFC), set_memoryw(0xFFFC, 0x0400)},
            .expected = {set_pc(0x0400)}
        }
    });
    // clang-format on

    static constexpr cpu_cycle_t indirect_cycle_cost = cpu_cycle_t(3);

    // clang-format off
    static const std::array indirect_behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_pc(0x0200), set_memoryw(0x0200, 0x0100), set_memoryw(0x0100, 0x0300)},
            .expected = {set_pc(0x0300)}
        },
        {
            .initial = {set_pc(0x0210), set_memoryw(0x0210, 0x01FF), set_memory(0x01FF, 0x00), set_memory(0x0100, 0x03)},
            .expected = {set_pc(0x0300)},
            .description = "JMP hardware bug"
        },
    });
    // clang-format on

    SECTION("absolute")
    {
        test_unspecified(cpu_opcode::jmp_absolute, absolute_behavior_scenarios, absolute_cycle_cost);
    }

    SECTION("indirect")
    {
        test_unspecified(cpu_opcode::jmp_indirect, indirect_behavior_scenarios, indirect_cycle_cost);
    }
}

TEST_CASE_METHOD(cpu_fixture, "jsr", "[cpu][instruction]")
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(6);

    // clang-format off
    static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_pc(0x0200), set_memoryw(0x0200, 0x0100)},
            .expected = {set_pc(0x0100), push_stackw(0x0201)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_pc(0x0200), set_memoryw(0x0200, 0x8000)},
            .expected = {set_pc(0x8000), push_stackw(0x0201)},
            .cycle_cost = cycle_cost
        },
        
        {
            .initial = {set_pc(0x01FF), set_memoryw(0x01FF, 0x0000)},
            .expected = {set_pc(0x0000), push_stackw(0x0200)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_pc(0x0200), set_memoryw(0x0200, 0xFFFF)},
            .expected = {set_pc(0xFFFF), push_stackw(0x0201)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_pc(0xF000), set_memoryw(0xF000, 0x4000)},
            .expected = {set_pc(0x4000), push_stackw(0xF001)},
            .cycle_cost = cycle_cost
        },

        {
            .initial = {set_pc(0x0000), set_memoryw(0x0000, 0x0200)},
            .expected = {set_pc(0x0200), push_stackw(0x0001)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_pc(0x8000), set_memoryw(0x8000, 0x0300)},
            .expected = {set_pc(0x0300), push_stackw(0x8001)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_pc(0xFFFC), set_memoryw(0xFFFC, 0x0400)},
            .expected = {set_pc(0x0400), push_stackw(0xFFFD)},
            .cycle_cost = cycle_cost
        }
    });
    // clang-format on

    test_unspecified(cpu_opcode::jsr_absolute, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_load_fixture, "lda", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = cpu_load_fixture::addr_mode_scenario<cpu_register_id::a>;
    const auto& behavior_scenarios = cpu_load_fixture::behavior_scenarios<cpu_register_id::a>;

    test_immediate(cpu_opcode::lda_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(cpu_opcode::lda_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::lda_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::lda_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::lda_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::lda_absolute_y, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::lda_indexed_indirect, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(cpu_opcode::lda_indirect_indexed, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_load_fixture, "ldx", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = cpu_load_fixture::addr_mode_scenario<cpu_register_id::x>;
    const auto& behavior_scenarios = cpu_load_fixture::behavior_scenarios<cpu_register_id::x>;

    test_immediate(cpu_opcode::ldx_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(cpu_opcode::ldx_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::ldx_zero_page_y, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::ldx_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::ldx_absolute_y, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_load_fixture, "ldy", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = cpu_load_fixture::addr_mode_scenario<cpu_register_id::y>;
    const auto& behavior_scenarios = cpu_load_fixture::behavior_scenarios<cpu_register_id::y>;

    test_immediate(cpu_opcode::ldy_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(cpu_opcode::ldy_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::ldy_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::ldy_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::ldy_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "lsr", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_operand(0x00)},
        .expected = {set_operand(0x00), set_status_zero()}
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        // flag carry = (original value & 0x1)
        {
            .initial = {set_operand(0x11)},
            .expected = {set_operand(0x08), set_status_carry()}
        },
        {
            .initial = {set_operand(0x1)},
            .expected = {set_operand(0x00), set_status_carry(), set_status_zero()}
        },        
        // no flag
        {
            .initial = {set_operand(0b00000100)},
            .expected = {set_operand(0b00000010)}
        },
        {
            .initial = {set_operand(0b01100110)},
            .expected = {set_operand(0b00110011)}
        },
    });
    // clang-format on

    test_acculumator(cpu_opcode::lsr_accumulator, behavior_scenarios);
    test_zero_page(cpu_opcode::lsr_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::lsr_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::lsr_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::lsr_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "nop", "[cpu][instruction]")
{
    static constexpr auto cycle_cost = cpu_cycle_t(2);

    // clang-format off
    static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_pc(default_pc_addr)},
            .expected = {}
        },
        {
            .initial = {set_pc(0x1234)},
            .expected = {}
        },
        {
            .initial = {set_pc(0x4321)},
            .expected = {}
        },
    });
    // clang-format on

    test_implied(cpu_opcode::nop_implied, behavior_scenarios, cycle_cost);
}

TEST_CASE_METHOD(cpu_fixture, "ora", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_a(0), set_operand(0)},
        .expected = {set_a(0), set_status_zero()}
    };
    
    static const std::array behavior_scenarios = std::to_array<scenario>({
        // zero, nothing to do
        
        // negative
        {
            .initial = {set_a(0x80), set_operand(0x01)},
            .expected = {set_a(0x81), set_status_negative()},
        },
        {
            .initial = {set_a(0x01), set_operand(0x80)},
            .expected = {set_a(0x81), set_status_negative()},
        },
        {
            .initial = {set_a(0x80), set_operand(0x80)},
            .expected = {set_a(0x80), set_status_negative()},
        },
        {
            .initial = {set_a(0xFF), set_operand(0x00)},
            .expected = {set_a(0xFF), set_status_negative()},
        },
        {
            .initial = {set_a(0x00), set_operand(0xFF)},
            .expected = {set_a(0xFF), set_status_negative()},
        },
        {
            .initial = {set_a(0xC0), set_operand(0x3F)},
            .expected = {set_a(0xFF), set_status_negative()},
        },
        {
            .initial = {set_a(0x3F), set_operand(0xC0)},
            .expected = {set_a(0xFF), set_status_negative()},
        },
        
        // positive
        {
            .initial = {set_a(0x7F), set_operand(0x00)},
            .expected = {set_a(0x7F)},
        },
        {
            .initial = {set_a(0x00), set_operand(0x7F)},
            .expected = {set_a(0x7F)},
        },
        {
            .initial = {set_a(0x3F), set_operand(0x40)},
            .expected = {set_a(0x7F)},
        },
        {
            .initial = {set_a(0x40), set_operand(0x3F)},
            .expected = {set_a(0x7F)},
        },
        {
            .initial = {set_a(0x1F), set_operand(0x20)},
            .expected = {set_a(0x3F)},
        },
        {
            .initial = {set_a(0x20), set_operand(0x1F)},
            .expected = {set_a(0x3F)},
        },
        
    });
    // clang-format off

    test_immediate(cpu_opcode::ora_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(cpu_opcode::ora_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::ora_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::ora_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::ora_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::ora_absolute_y, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::ora_indexed_indirect, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(cpu_opcode::ora_indirect_indexed, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "pha", "[cpu][instruction]")
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(3);

    // clang-format off
    static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_sp(0x00), set_a(0x00)},
            .expected = {set_sp(0xFF), set_stack(0x00, 0x00)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_a(0xFF)},
            .expected = {set_sp(0xFF), set_stack(0x00, 0xFF)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_a(0x80)},
            .expected = {set_sp(0xFF), set_stack(0x00, 0x80)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_a(0x55)},
            .expected = {set_sp(0xFF), set_stack(0x00, 0x55)},
            .cycle_cost = cycle_cost
        },
    });
    // clang-format on

    test_implied(cpu_opcode::pha_implied, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "php", "[cpu][instruction]")
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(3);
    static constexpr cpu_status mandatory_flags = static_cast<cpu_status>(0x30);

    // clang-format off
    static const std::array behavior_scenarios = std::to_array<scenario>({
        // set flag
        {
            .initial = {set_sp(0x00), set_status(cpu_status::carry)},
            .expected = {set_sp(0xFF), set_stack(0x00, static_cast<byte_t>(cpu_status::carry | mandatory_flags))},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_status(cpu_status::zero)},
            .expected = {set_sp(0xFF), set_stack(0x00, static_cast<byte_t>(cpu_status::zero | mandatory_flags))},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_status(cpu_status::interrupt)},
            .expected = {set_sp(0xFF), set_stack(0x00, static_cast<byte_t>(cpu_status::interrupt | mandatory_flags))},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_status(cpu_status::decimal)},
            .expected = {set_sp(0xFF), set_stack(0x00, static_cast<byte_t>(cpu_status::decimal | mandatory_flags))},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_status(cpu_status::unused)},
            .expected = {set_sp(0xFF), set_stack(0x00, static_cast<byte_t>(cpu_status::unused | mandatory_flags))},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_status(cpu_status::overflow)},
            .expected = {set_sp(0xFF), set_stack(0x00, static_cast<byte_t>(cpu_status::overflow | mandatory_flags))},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_status(cpu_status::negative)},
            .expected = {set_sp(0xFF), set_stack(0x00, static_cast<byte_t>(cpu_status::negative | mandatory_flags))},
            .cycle_cost = cycle_cost
        },

        // ignore flag
        {
            .initial = {set_sp(0x00), set_status(cpu_status::break_cmd)},
            .expected = {set_sp(0xFF), set_stack(0x00, static_cast<byte_t>(mandatory_flags))},
            .cycle_cost = cycle_cost
        }
    });
    // clang-format on

    test_implied(cpu_opcode::php_implied, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "pla", "[cpu][instruction]")
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

    // clang-format off
    static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_sp(0x00), set_stack(0x01, 0x01)},
            .expected = {set_sp(0x01), set_a(0x01)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0xFF), set_stack(0x00, 0x01)},
            .expected = {set_sp(0x00), set_a(0x01)},
            .cycle_cost = cycle_cost
        },

        // zero
        {
            .initial = {set_sp(0xFF), set_stack(0x00, 0x00)},
            .expected = {set_sp(0x00), set_a(0x00), set_status_zero()},
            .cycle_cost = cycle_cost
        },

        // negative
        {
            .initial = {set_sp(0x00), set_stack(0x01, 0x80)},
            .expected = {set_sp(0x01), set_a(0x80), set_status_negative()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, 0xC0)},
            .expected = {set_sp(0x01), set_a(0xC0), set_status_negative()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, 0xFF)},
            .expected = {set_sp(0x01), set_a(0xFF), set_status_negative()},
            .cycle_cost = cycle_cost
        },

        // positive
        {
            .initial = {set_sp(0x00), set_stack(0x01, 0x10)},
            .expected = {set_sp(0x01), set_a(0x10)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, 0x40)},
            .expected = {set_sp(0x01), set_a(0x40)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, 0x7F)},
            .expected = {set_sp(0x01), set_a(0x7F)},
            .cycle_cost = cycle_cost
        }
    });
    // clang-format on

    test_implied(cpu_opcode::pla_implied, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "plp", "[cpu][instruction]")
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

    // clang-format off
    static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_sp(0x00), set_stack(0x01, 0x00)},
            .expected = {set_sp(0x01), set_status(cpu_status::unused)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0xFF), set_stack(0x00, 0x00)},
            .expected = {set_sp(0x00), set_status(cpu_status::unused)},
            .cycle_cost = cycle_cost
        },
        
        // flag set
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::carry))},
            .expected = {set_sp(0x01), set_status(cpu_status::carry | cpu_status::unused)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::decimal))},
            .expected = {set_sp(0x01), set_status(cpu_status::decimal | cpu_status::unused)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::unused))},
            .expected = {set_sp(0x01), set_status(cpu_status::unused)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::overflow))},
            .expected = {set_sp(0x01), set_status(cpu_status::overflow | cpu_status::unused)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::negative))},
            .expected = {set_sp(0x01), set_status(cpu_status::negative | cpu_status::unused)},
            .cycle_cost = cycle_cost
        },
        
        // flag ignored
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::break_cmd))},
            .expected = {set_sp(0x01), set_status(cpu_status::unused)},
            .cycle_cost = cycle_cost
        }
    });
    // clang-format on

    test_implied(cpu_opcode::plp_implied, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "rol", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_operand(0x00)},
        .expected = {set_operand(0x00), set_status_zero()}
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        // carry flag initial clear
        // flag carry
        {
            .initial = {set_operand(0b10000000)},
            .expected = {set_operand(0b00000000), set_status_carry(), set_status_zero()}
        },
        {
            .initial = {set_operand(0b10000001)},
            .expected = {set_operand(0b00000010), set_status_carry()}
        },
        // flag negative
        {
            .initial = {set_operand(0b01000000)},
            .expected = {set_operand(0b10000000), set_status_negative()}
        },
        {
            .initial = {set_operand(0b01001000)},
            .expected = {set_operand(0b10010000), set_status_negative()}
        },
        // no flag
        {
            .initial = {set_operand(0b00000100)},
            .expected = {set_operand(0b00001000)}
        },

        // carry flag initial set
        // flag carry
        {
            .initial = {set_operand(0b10000000), set_status_carry()},
            .expected = {set_operand(0b00000001), set_status_carry()}
        },
        {
            .initial = {set_operand(0b10000001), set_status_carry()},
            .expected = {set_operand(0b00000011), set_status_carry()}
        },
        // flag negative
        {
            .initial = {set_operand(0b01000000), set_status_carry()},
            .expected = {set_operand(0b10000001), clear_status_carry(), set_status_negative()}
        },
        {
            .initial = {set_operand(0b01001000), set_status_carry()},
            .expected = {set_operand(0b10010001), clear_status_carry(),set_status_negative()}
        },
        // no flag
        {
            .initial = {set_operand(0b00000100), set_status_carry()},
            .expected = {set_operand(0b00001001), clear_status_carry()}
        },

    });
    // clang-format on

    test_acculumator(cpu_opcode::rol_accumulator, behavior_scenarios);
    test_zero_page(cpu_opcode::rol_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::rol_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::rol_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::rol_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "ror", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_operand(0x00)},
        .expected = {set_operand(0x00), set_status_zero()}
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        // carry flag initial clear
        // flag | carry = (original value & 0x1)
        {
            .initial = {set_operand(0x11)},
            .expected = {set_operand(0x08), set_status_carry()}
        },
        {
            .initial = {set_operand(0x1)},
            .expected = {set_operand(0x00), set_status_carry(), set_status_zero()}
        },        
        // no flag
        {
            .initial = {set_operand(0b00000100)},
            .expected = {set_operand(0b00000010)}
        },
        {
            .initial = {set_operand(0b01100110)},
            .expected = {set_operand(0b00110011)}
        },

        // carry flag initial set
        // flag | carry = (original value & 0x1)
        {
            .initial = {set_operand(0x11), set_status_carry()},
            .expected = {set_operand(0x88), set_status_carry(), set_status_negative()}
        },
        {
            .initial = {set_operand(0x1), set_status_carry()},
            .expected = {set_operand(0x80), set_status_carry(), set_status_negative()}
        },        
        // no flag
        {
            .initial = {set_operand(0b00000100), set_status_carry(), set_status_negative()},
            .expected = {set_operand(0b10000010), clear_status_carry()}
        },
        {
            .initial = {set_operand(0b01100110), set_status_carry(), set_status_negative()},
            .expected = {set_operand(0b10110011), clear_status_carry()}
        },
    });
    // clang-format on

    test_acculumator(cpu_opcode::ror_accumulator, behavior_scenarios);
    test_zero_page(cpu_opcode::ror_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::ror_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::ror_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::ror_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "rti", "[cpu][instruction]")
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(6);

    // clang-format off
    static const std::array behavior_scenarios = std::to_array<scenario>({
        // addr
        {
            .initial = {set_sp(0xFC), set_stack(0xFD, 0x00), set_stackw(0xFE, 0x4000)},
            .expected = {set_pc(0x4000), set_sp(0xFF), set_status_unused()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0xFD), set_stack(0xFE, 0x00), set_stackw(0xFF, 0x1234)},
            .expected = {set_pc(0x1234), set_sp(0x00), set_status_unused()},
            .description = "SP at FE to simulate edge case of wrapping around from 0x01FF to 0x0100",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0xFE), set_stack(0xFF, 0x00), set_stackw(0x00, 0x0200)},
            .expected = {set_pc(0x0200), set_sp(0x01), set_status_unused()},
            .cycle_cost = cycle_cost
        },

        // flag set
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::carry)), set_stackw(0x02, 0x0200)},
            .expected = {set_pc(0x0200), set_sp(0x03), set_status_carry(), set_status_unused()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::decimal)), set_stackw(0x02, 0x0200)},
            .expected = {set_pc(0x0200), set_sp(0x03), set_status_decimal(), set_status_unused()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::interrupt)), set_stackw(0x02, 0x0200)},
            .expected = {set_pc(0x0200), set_sp(0x03), set_status_interrupt(), set_status_unused()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::negative)), set_stackw(0x02, 0x0200)},
            .expected = {set_pc(0x0200), set_sp(0x03), set_status_negative(), set_status_unused()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::overflow)), set_stackw(0x02, 0x0200)},
            .expected = {set_pc(0x0200), set_sp(0x03), set_status_overflow(), set_status_unused()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::unused)), set_stackw(0x02, 0x0200)},
            .expected = {set_pc(0x0200), set_sp(0x03), set_status_unused()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::zero)), set_stackw(0x02, 0x0200)},
            .expected = {set_pc(0x0200), set_sp(0x03), set_status_zero(), set_status_unused()},
            .cycle_cost = cycle_cost
        },

        // flag ignore
        {
            .initial = {set_sp(0x00), set_stack(0x01, static_cast<byte_t>(cpu_status::break_cmd)), set_stackw(0x02, 0x0200)},
            .expected = {set_pc(0x0200), set_sp(0x03), set_status_unused()},
            .cycle_cost = cycle_cost
        },
    });
    // clang-format on

    test_implied(cpu_opcode::rti_implied, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "rts", "[cpu][instruction]")
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(6);

    // clang-format off
    static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_sp(0xFD), set_stack(0xFE, 0x00), set_stack(0xFF, 0x40)},
            .expected = {set_pc(0x4001), set_sp(0xFF)},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0xFE), set_stack(0xFF, 0x34), set_stack(0x00, 0x12)},
            .expected = {set_pc(0x1235), set_sp(0x00)},
            .description = "SP at FE to simulate edge case of wrapping around from 0x01FF to 0x0100",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_sp(0xFF), set_stack(0x00, 0x00), set_stack(0x01, 0x02)},
            .expected = {set_pc(0x0201), set_sp(0x01)},
            .cycle_cost = cycle_cost
        }
    });
    // clang-format on

    test_implied(cpu_opcode::rts_implied, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "sbc", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_a(0x00), set_operand(0x00), set_status_carry()},
        .expected = {set_status_zero()}
    };
    
    static const std::array behavior_scenarios = std::to_array<scenario>({
        // carry flag initially clear
        {
            .initial = {set_a(0x00), set_operand(0x00)},
            .expected = {set_a(0xFF), set_status_negative()}
        },
        {
            .initial = {set_a(0x01), set_operand(0x00)},
            .expected = {set_a(0x00), set_status_carry(), set_status_zero()}
        },
        {
            .initial = {set_a(0x02), set_operand(0x01)},
            .expected = {set_a(0x00), set_status_carry(), set_status_zero()}
        },
        {
            .initial = {set_a(0x80), set_operand(0x7F)},
            .expected = {set_a(0x00), set_status_carry(), set_status_zero(), set_status_overflow()}
        },
        {
            .initial = {set_a(0x80), set_operand(0x80)},
            .expected = {set_a(0xFF), set_status_negative()}
        },

        // carry flag initially set
        {
            .initial = {set_a(0x00), set_operand(0x00), set_status_carry()},
            .expected = {set_a(0x00), set_status_zero()}
        },
        {
            .initial = {set_a(0x01), set_operand(0x01), set_status_carry()},
            .expected = {set_a(0x00), set_status_zero()}
        },
        {
            .initial = {set_a(0x02), set_operand(0x01), set_status_carry()},
            .expected = {set_a(0x01)}
        },
        {
            .initial = {set_a(0x7F), set_operand(0x01), set_status_carry()},
            .expected = {set_a(0x7E)}
        },
        {
            .initial = {set_a(0x80), set_operand(0x7F), set_status_carry()},
            .expected = {set_a(0x01), set_status_overflow()}
        },
        {
            .initial = {set_a(0x80), set_operand(0x80), set_status_carry()},
            .expected = {set_a(0x00), set_status_zero()}
        },
        {
            .initial = {set_a(0x7E), set_operand(0x7F), set_status_carry()},
            .expected = {set_a(0xFF), set_status_negative(), clear_status_carry()}
        },
        {
            .initial = {set_a(0xFF), set_operand(0x7E), set_status_carry()},
            .expected = {set_a(0x81), set_status_negative()}
        },
        {
            .initial = {set_a(0x00), set_operand(0xFF), set_status_carry()},
            .expected = {set_a(0x01), clear_status_carry()}
        },
    });
    // clang-format on

    test_immediate(cpu_opcode::sbc_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(cpu_opcode::sbc_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::sbc_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::sbc_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::sbc_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::sbc_absolute_y, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::sbc_indexed_indirect, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(cpu_opcode::sbc_indirect_indexed, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_set_status_fixture, "sec", "[cpu][instruction]")
{
    test_implied(cpu_opcode::sec_implied, behavior_scenarios<cpu_status::carry>);
}

TEST_CASE_METHOD(cpu_set_status_fixture, "sed", "[cpu][instruction]")
{
    test_implied(cpu_opcode::sed_implied, behavior_scenarios<cpu_status::decimal>);
}

TEST_CASE_METHOD(cpu_set_status_fixture, "sei", "[cpu][instruction]")
{
    test_implied(cpu_opcode::sei_implied, behavior_scenarios<cpu_status::interrupt>);
}

TEST_CASE_METHOD(cpu_store_fixture, "sta", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = cpu_store_fixture::addr_mode_scenario<cpu_register_id::a>;
    const auto& behavior_scenarios = cpu_store_fixture::behavior_scenarios<cpu_register_id::a>;

    test_zero_page(cpu_opcode::sta_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::sta_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::sta_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::sta_absolute_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::sta_absolute_y, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::sta_indexed_indirect, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_store_fixture, "stx", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = cpu_store_fixture::addr_mode_scenario<cpu_register_id::x>;
    const auto& behavior_scenarios = cpu_store_fixture::behavior_scenarios<cpu_register_id::x>;

    test_zero_page(cpu_opcode::stx_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::stx_zero_page_y, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::stx_absolute, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_store_fixture, "sty", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = cpu_store_fixture::addr_mode_scenario<cpu_register_id::y>;
    const auto& behavior_scenarios = cpu_store_fixture::behavior_scenarios<cpu_register_id::y>;

    test_zero_page(cpu_opcode::sty_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::sty_zero_page_x, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::sty_absolute, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_transfer_fixture, "tax", "[cpu][instruction]")
{
    test_implied(cpu_opcode::tax_implied, behavior_scenarios<cpu_register_id::a, cpu_register_id::x>);
}

TEST_CASE_METHOD(cpu_transfer_fixture, "tay", "[cpu][instruction]")
{
    test_implied(cpu_opcode::tay_implied, behavior_scenarios<cpu_register_id::a, cpu_register_id::y>);
}

TEST_CASE_METHOD(cpu_transfer_fixture, "tsx", "[cpu][instruction]")
{
    test_implied(cpu_opcode::tsx_implied, behavior_scenarios<cpu_register_id::sp, cpu_register_id::x>);
}

TEST_CASE_METHOD(cpu_transfer_fixture, "txa", "[cpu][instruction]")
{
    test_implied(cpu_opcode::txa_implied, behavior_scenarios<cpu_register_id::x, cpu_register_id::a>);
}

TEST_CASE_METHOD(cpu_transfer_fixture, "txs", "[cpu][instruction]")
{
    test_implied(cpu_opcode::txs_implied, behavior_scenarios<cpu_register_id::x, cpu_register_id::sp, false>);
}

TEST_CASE_METHOD(cpu_transfer_fixture, "tya", "[cpu][instruction]")
{
    test_implied(cpu_opcode::tya_implied, behavior_scenarios<cpu_register_id::y, cpu_register_id::a>);
}


#if NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED

TEST_CASE_METHOD(cpu_fixture, "dcp", "[cpu][instruction]")
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_operand(0x02), set_a(0x00)},
        .expected = {set_operand(0x01), set_status_negative()},
        .cycle_cost = cycle_cost
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_operand(0x00), set_a(0x00)},
            .expected = {set_operand(0xFF)},
            .description = "0x00 results in overflow to 0xFF",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_operand(0x80), set_a(0x00)},
            .expected = {set_operand(0x7F), set_status_negative()},
            .description = "0x80 results in overflow to 0x7F",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_operand(0x01), set_a(0x00)},
            .expected = {set_operand(0x00), set_status_carry(), set_status_zero()},
            .description = "zero",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_operand(0xC1), set_a(0x00)},
            .expected = {set_operand(0xC0)},
            .description = "negative value",
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_operand(0x41), set_a(0x00)},
            .expected = {set_operand(0x40), set_status_negative()},
            .description = "positive value",
            .cycle_cost = cycle_cost
        },
        
        {
            .initial = {set_operand(0x01), set_a(0x01)},
            .expected = {set_operand(0x00), set_status_carry()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_operand(0x80), set_a(0x80)},
            .expected = {set_operand(0x7F), set_status_carry()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_operand(0xFF), set_a(0xFF)},
            .expected = {set_operand(0xFE), set_status_carry()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_operand(0x00), set_a(0xFE)},
            .expected = {set_operand(0xFF), set_status_negative()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_operand(0xFF), set_a(0xFE)},
            .expected = {set_operand(0xFE), set_status_carry(), set_status_zero()},
            .cycle_cost = cycle_cost
        },
        {
            .initial = {set_operand(0x81), set_a(0x80)},
            .expected = {set_operand(0x80), set_status_carry(), set_status_zero()},
            .cycle_cost = cycle_cost
        },
    });
    // clang-format on

    test_zero_page(cpu_opcode::dcp_zero_page_unofficial, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::dcp_zero_page_x_unofficial, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::dcp_absolute_unofficial, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::dcp_absolute_x_unofficial, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::dcp_absolute_y_unofficial, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::dcp_indexed_indirect_unofficial, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(cpu_opcode::dcp_indirect_indexed_unofficial, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "isb", "[cpu][instruction]")
{
    static constexpr cpu_cycle_t base_cycle_cost = cpu_cycle_t(2);

    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_operand(0xFF), set_a(0x00), set_status_carry()},
        .expected = {set_operand(0x00), set_status_zero()},
        .cycle_cost = base_cycle_cost
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        // carry flag initially clear
        {
            .initial = {set_operand(0xFF), set_a(0x00)},
            .expected = {set_operand(0x00), set_a(0xFF), set_status_negative()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0xFF), set_a(0x01)},
            .expected = {set_operand(0x00), set_a(0x00), set_status_carry(), set_status_zero()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0x00), set_a(0x02)},
            .expected = {set_operand(0x01), set_a(0x00), set_status_carry(), set_status_zero()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0x7E), set_a(0x80)},
            .expected = {set_operand(0x7F), set_a(0x00), set_status_carry(), set_status_zero(), set_status_overflow()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0x7F), set_a(0x80)},
            .expected = {set_operand(0x80), set_a(0xFF), set_status_negative()},
            .cycle_cost = base_cycle_cost
        },
        
        // carry flag initially set
        {
            .initial = {set_operand(0xFF), set_a(0x00), set_status_carry()},
            .expected = {set_operand(0x00), set_a(0x00), set_status_zero()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0x00), set_a(0x01), set_status_carry()},
            .expected = {set_operand(0x01), set_a(0x00), set_status_zero()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0x00), set_a(0x02), set_status_carry()},
            .expected = {set_operand(0x01), set_a(0x01)},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0x00), set_a(0x7F), set_status_carry()},
            .expected = {set_operand(0x01), set_a(0x7E)},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0x7E), set_a(0x80), set_status_carry()},
            .expected = {set_operand(0x7F), set_a(0x01), set_status_overflow()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0x7F), set_a(0x80), set_status_carry()},
            .expected = {set_operand(0x80), set_a(0x00), set_status_zero()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0x7E), set_a(0x7E), set_status_carry()},
            .expected = {set_operand(0x7F), set_a(0xFF), set_status_negative(), clear_status_carry()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0x7D), set_a(0xFF), set_status_carry()},
            .expected = {set_operand(0x7E), set_a(0x81), set_status_negative()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0xFE), set_a(0x00), set_status_carry()},
            .expected = {set_operand(0xFF), set_a(0x01), clear_status_carry()},
            .cycle_cost = base_cycle_cost
        },
    });
    // clang-format on

    test_zero_page(cpu_opcode::isb_zero_page_unofficial, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::isb_zero_page_x_unofficial, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::isb_absolute_unofficial, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::isb_absolute_x_unofficial, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::isb_absolute_y_unofficial, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::isb_indexed_indirect_unofficial, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(cpu_opcode::isb_indirect_indexed_unofficial, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "lax", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_operand(1)},
        .expected = {set_a(1), set_x(1)}
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_operand(0x00)},
            .expected = {set_a(0x00), set_x(0x00), set_status_zero()},
            .description = "zero value"
        },
        {
            .initial = {set_operand(0x01)},
            .expected = {set_a(0x01), set_x(0x01)},
            .description = "smallest positive value"
        },
        {
            .initial = {set_operand(0x40)},
            .expected = {set_a(0x40), set_x(0x40)},
            .description = "mid positive value"
        },
        {
            .initial = {set_operand(0x7F)},
            .expected = {set_a(0x7F), set_x(0x7F)},
            .description = "largest positive value"
        },
        {
            .initial = {set_operand(0xFF)},
            .expected = {set_a(0xFF), set_x(0xFF), set_status_negative()},
            .description = "smallest negative value"
        },
        {
            .initial = {set_operand(0xC0)},
            .expected = {set_a(0xC0), set_x(0xC0), set_status_negative()},
            .description = "mid negative value"
        },
        {
            .initial = {set_operand(0x80)},
            .expected = {set_a(0x80), set_x(0x80), set_status_negative()},
            .description = "largest negative value"
        }
    });
    // clang-format on

    test_zero_page(cpu_opcode::lax_zero_page_unofficial, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::lax_zero_page_y_unofficial, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::lax_absolute_unofficial, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::lax_absolute_y_unofficial, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::lax_indexed_indirect_unofficial, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(cpu_opcode::lax_indirect_indexed_unofficial, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "sax", "[cpu][instruction]")
{
    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_a(0x00), set_x(0x00)},
        .expected = {set_operand(0x00)}
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        // zero
        {
            .initial = {set_a(0x00), set_x(0xFF)},
            .expected = {set_operand(0x00)},
        },
        {
            .initial = {set_a(0xFF), set_x(0x00)},
            .expected = {set_operand(0x00)},
        },
        {
            .initial = {set_a(0x0F), set_x(0xF0)},
            .expected = {set_operand(0x00)},
        },
        
        // negative
        {
            .initial = {set_a(0x80), set_x(0xFF)},
            .expected = {set_operand(0x80)},
        },
        {
            .initial = {set_a(0xFF), set_x(0x80)},
            .expected = {set_operand(0x80)},
        },
        {
            .initial = {set_a(0xC0), set_x(0xF0)},
            .expected = {set_operand(0xC0)},
        },
        {
            .initial = {set_a(0xF0), set_x(0xC0)},
            .expected = {set_operand(0xC0)},
        },
        {
            .initial = {set_a(0x80), set_x(0x80)},
            .expected = {set_operand(0x80)},
        },
        {
            .initial = {set_a(0xFF), set_x(0xC0)},
            .expected = {set_operand(0xC0)},
        },
        
        // positive
        {
            .initial = {set_a(0x7F), set_x(0xFF)},
            .expected = {set_operand(0x7F)},
        },
        {
            .initial = {set_a(0x3F), set_x(0x7F)},
            .expected = {set_operand(0x3F)},
        },
        {
            .initial = {set_a(0x1F), set_x(0x2F)},
            .expected = {set_operand(0x0F)},
        },
        {
            .initial = {set_a(0x0F), set_x(0x8F)},
            .expected = {set_operand(0x0F)},
        },
        {
            .initial = {set_a(0x7F), set_x(0x3F)},
            .expected = {set_operand(0x3F)},
        },
        {
            .initial = {set_a(0x2F), set_x(0x1F)},
            .expected = {set_operand(0x0F)},
        },
        {
            .initial = {set_a(0x01), set_x(0x01)},
            .expected = {set_operand(0x01)},
        }
    });
    // clang-format on

    test_zero_page(cpu_opcode::sax_zero_page_unofficial, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::sax_zero_page_y_unofficial, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::sax_absolute_unofficial, addr_mode_scenario, behavior_scenarios);

    // TODO test indexed_indirect
    // test_indexed_indirect(cpu_opcode::sax_indexed_indirect_unofficial, addr_mode_scenario, behavior_scenarios);
}

TEST_CASE_METHOD(cpu_fixture, "slo", "[cpu][instruction]")
{
    static constexpr cpu_cycle_t base_cycle_cost = cpu_cycle_t(2);

    // clang-format off
    static const scenario addr_mode_scenario{
        .initial = {set_operand(0x00), set_a(0x00)},
        .expected = {set_operand(0x00), set_status_zero()},
        .cycle_cost = base_cycle_cost
    };

    static const std::array behavior_scenarios = std::to_array<scenario>({
        // flag carry = (original value & 0x80)
        {
            .initial = {set_operand(0b10000000), set_a(0x00)},
            .expected = {set_operand(0b00000000), set_status_carry(), set_status_zero()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0b10000001), set_a(0x00)},
            .expected = {set_operand(0b00000010), set_a(0b00000010), set_status_carry()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0b10000001), set_a(0b00000100)},
            .expected = {set_operand(0b00000010), set_a(0b00000110), set_status_carry()},
            .cycle_cost = base_cycle_cost
        },

        // flag negative
        {
            .initial = {set_operand(0b01000000), set_a(0x00)},
            .expected = {set_operand(0b10000000), set_a(0b10000000), set_status_negative()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0b01001000), set_a(0x00)},
            .expected = {set_operand(0b10010000), set_a(0b10010000), set_status_negative()},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0b01001000), set_a(0b00000100)},
            .expected = {set_operand(0b10010000), set_a(0b10010100), set_status_negative()},
            .cycle_cost = base_cycle_cost
        },

        // no flag
        {
            .initial = {set_operand(0b00000100), set_a(0x00)},
            .expected = {set_operand(0b00001000), set_a(0b00001000)},
            .cycle_cost = base_cycle_cost
        },
        {
            .initial = {set_operand(0b00000100), set_a(0b00000100)},
            .expected = {set_operand(0b00001000), set_a(0b00001100)},
            .cycle_cost = base_cycle_cost
        },
    });
    // clang-format on

    test_zero_page(cpu_opcode::slo_zero_page_unofficial, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(cpu_opcode::slo_zero_page_x_unofficial, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(cpu_opcode::slo_absolute_unofficial, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::slo_absolute_x_unofficial, cpu_register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(cpu_opcode::slo_absolute_y_unofficial, cpu_register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(cpu_opcode::slo_indexed_indirect_unofficial, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(cpu_opcode::slo_indirect_indexed_unofficial, addr_mode_scenario, behavior_scenarios);
}

#endif

} // namespace nese
