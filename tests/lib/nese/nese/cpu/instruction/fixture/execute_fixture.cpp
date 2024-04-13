#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/execute.hpp>
#include <nese/cpu/instruction/execute_context.hpp>

namespace nese::cpu::instruction {

static constexpr std::array zero_page_scenarios = std::to_array<std::tuple<addr_x, byte_x>>(
    {
        /* Simple zero-page tests */
        {0x0200, 0x00}, /* PC in a common code area, val_addr at zero-page start */
        {0x0200, 0xFF}, /* PC in a common code area, val_addr at zero-page end */
        /* Boundary conditions */
        {0x01FF, 0x00}, /* PC just before zero page, val_addr at zero page start */
        {0x0200, 0x01}, /* PC in a common code area, val_addr just into zero page */
        {0x01FF, 0xFF}, /* PC just before zero page, val_addr at zero page end */
        {0xF000, 0x80}, /* Higher PC value, val_addr in middle of zero page */
        /* Testing PC at various points */
        {0x0000, 0x02}, /* PC at start of memory, testing very early execution */
        {0x8000, 0x04}, /* PC in a typical ROM area, val_addr early in zero page */
        {0xFFFC, 0xFE}, /* PC at the very end of memory space */
        /* Varied val_addr values to test LDA, LDX, LDY behavior */
        {0x0300, 0x10}, /* Common code area, testing nonzero page value */
        {0x0400, 0x20}  /* Another common code area, testing nonzero page value */
    });

static constexpr std::array zero_page_indexed_scenarios = std::to_array<std::tuple<addr_x, byte_x, byte_x>>(
    {
        // Offset within zero-page without wrap: Tests basic indexed addressing within the zero page without crossing the zero-page boundary.
        {0x0200, 0x00, 0x01}, // PC in common area, base at start of zero page, offset +1.
        {0x0200, 0x10, 0x0F}, // PC in common area, base within zero page, offset to edge without wrapping.

        // Offset causing wrap-around: Validates correct handling of address wrap-around within the zero-page (255 + offset wraps to start of zero page).
        {0x0200, 0xFE, 0x01}, // PC in common area, base near end of zero page, +1 offset wraps.
        {0x0200, 0xFD, 0x03}, // PC in common area, base within zero page, +3 offset wraps.

        // Varied PC values with offset: Tests how indexed zero-page addressing behaves with different PC values, ensuring PC location doesn't affect indexed addressing calculation.
        {0x0000, 0x02, 0x01}, // PC at memory start, base early in zero page, small offset.
        {0x8000, 0x03, 0x02}, // PC in typical ROM area, base early in zero page, small offset.
        {0xFFFC, 0xFD, 0x02}  // PC at end of memory space, base in zero page, offset without wrap.
    });

static constexpr std::array absolute_scenarios = std::to_array<std::tuple<addr_x, addr_x>>(
    {
        /* Absolute addressing tests in different memory regions */
        {0x0200, 0x0100}, /* PC in common code area, absolute address in lower memory */
        {0x0200, 0x8000}, /* PC in common code area, absolute address in upper memory */
        /* Boundary conditions */
        {0x01FF, 0x0000}, /* PC just before zero page, testing absolute address at memory start */
        {0x0200, 0xFFFF}, /* PC in common code area, testing absolute address at memory end */
        {0xF000, 0x4000}, /* Higher PC value, absolute address in the middle of memory */
        /* Testing PC at various points */
        {0x0000, 0x0200}, /* PC at start of memory, absolute address in common code area */
        {0x8000, 0x0300}, /* PC in a typical ROM area, absolute address in common code area */
        {0xFFFC, 0x0400}  /* PC at the very end of memory space, absolute address in common code area */
    });

static constexpr std::array absolute_indexed_scenarios = std::to_array<std::tuple<addr_x, addr_x, byte_x>>(
    {
        /* Conceptual tests with "offsets" for absolute addressing */
        {0x0200, 0x0100, 0x01}, /* Common code area, lower memory with a byte offset */
        {0x0200, 0x8000, 0x10}, /* Common code area, upper memory with a byte offset */
        /* Varied PC values with "offsets" */
        {0x0000, 0x0200, 0x20}, /* PC at start, absolute address in common code area with a byte offset */
        {0x8000, 0x0300, 0x30}, /* PC in ROM area, absolute address in common code area with a byte offset */
        {0xFFFC, 0x0400, 0x40}  /* PC at end, absolute address in common code area with a byte offset */
    });

static constexpr std::array indexed_indirect_scenarios = std::to_array<std::tuple<addr_x, byte_x, byte_x, byte_x>>(
    {
        // TODO Meaningful values
        // [pc_addr, base_addr, x, value_addr]
        {0x0200, 0x01, 0x01, 0x11},
        {0x0200, 0x80, 0x80, 0x10},
        {0x0000, 0x10, 0x2, 0x30}, 
        {0x8000, 0x03, 0x3, 0x40}, 
        {0xFFF9, 0x04, 0x4, 0x50}  
    });

string execute_fixture::scenario::to_string() const
{
    auto append_change_strings = [](auto& string, const auto& changes) {
        for (const auto& change_variant : changes)
        {
            string_view change_string;
            std::visit([&change_string](const auto& change) { change_string = change.to_string(); }, change_variant);

            if (!change_string.empty())
            {
                string += "\n    ";
                string += change_string;
            }
        }
    };

    string string;
    string.reserve(1024);

    string += "\n  ";
    string += "initial_changes := [";
    append_change_strings(string, initial_changes);
    string += "\n  ]";

    string += "\n  ";
    string += "expected_changes := [";
    append_change_strings(string, expected_changes);
    string += "\n  ]";

    return string;
}
void execute_fixture::test_acculumator(opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost)
{
    SECTION("accumulator")
    {
        const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

        INFO(scenario.description);

        CAPTURE(scenario);

        change_context context{default_pc_addr, addr_mode::accumulator, state, memory};
        apply_changes(scenario.initial_changes, context);

        expected_state = state;
        expected_memory = memory;

        expected_state.cycle = base_cycle_cost + scenario.base_cycle_cost;

        change_context expected_context{default_pc_addr, addr_mode::accumulator, expected_state, expected_memory};
        apply_changes(scenario.expected_changes, expected_context);

        execute_and_check(opcode);
    }
}

void execute_fixture::test_implied(opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost)
{
    SECTION("implied")
    {
        const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

        INFO(scenario.description);

        CAPTURE(scenario);

        change_context context{default_pc_addr, addr_mode::implied, state, memory};
        apply_changes(scenario.initial_changes, context);

        expected_state = state;
        expected_memory = memory;

        expected_state.cycle = base_cycle_cost + scenario.base_cycle_cost;

        change_context expected_context{default_pc_addr, addr_mode::implied, expected_state, expected_memory};
        apply_changes(scenario.expected_changes, expected_context);

        execute_and_check(opcode);
    }
}

void execute_fixture::test_immediate(opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost)
{
    auto test = [this, opcode, base_cycle_cost](addr_x pc_addr, const scenario& scenario) {
        CAPTURE(pc_addr);
        CAPTURE(scenario);

        state.registers.pc = pc_addr;

        change_context context{pc_addr, addr_mode::immediate, state, memory};
        apply_changes(scenario.initial_changes, context);

        expected_state = state;
        expected_memory = memory;

        expected_state.cycle = base_cycle_cost + scenario.base_cycle_cost;
        expected_state.registers.pc = pc_addr + 1;

        change_context expected_context{pc_addr, addr_mode::immediate, expected_state, expected_memory};
        apply_changes(scenario.expected_changes, expected_context);

        execute_and_check(opcode);
    };

    SECTION("immediate")
    {
        SECTION("addressing")
        {
            const addr_x pc_addr = GENERATE(as<addr_x>(), 0x0000, 0x7FFE, 0xFFFE);

            test(pc_addr, addressing_scenario);
        }

        SECTION("behavior")
        {
            const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

            INFO(scenario.description);

            test(default_pc_addr, scenario);
        }
    }
}

void execute_fixture::test_zero_page(opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost)
{
    auto test = [this, opcode, base_cycle_cost](addr_x pc_addr, byte_x base_addr, const scenario& scenario) {
        CAPTURE(pc_addr);
        CAPTURE(base_addr);
        CAPTURE(scenario);

        state.registers.pc = pc_addr;
        memory.set_byte(pc_addr, base_addr);

        change_context context{base_addr, addr_mode::zero_page, state, memory};
        apply_changes(scenario.initial_changes, context);

        expected_state = state;
        expected_memory = memory;

        expected_state.cycle = base_cycle_cost + scenario.base_cycle_cost;
        expected_state.registers.pc = pc_addr + 1;

        change_context expected_context{base_addr, addr_mode::zero_page, expected_state, expected_memory};
        apply_changes(scenario.expected_changes, expected_context);

        execute_and_check(opcode);
    };

    SECTION("zero_page")
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr] = GENERATE(from_range(zero_page_scenarios));

            test(pc_addr, base_addr, addressing_scenario);
        }

        SECTION("behavior")
        {
            const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

            INFO(scenario.description);

            test(default_pc_addr, zero_page_base_addr, scenario);
        }
    }
}

void execute_fixture::test_zero_page_indexed(opcode opcode, register_id index_register, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost)
{
    auto test = [this, opcode, index_register, base_cycle_cost](addr_x pc_addr, byte_x base_addr, byte_x index, const scenario& scenario) {
        const byte_x indexed_addr = base_addr + index & 0xff;

        CAPTURE(pc_addr);
        CAPTURE(base_addr);
        CAPTURE(index_register);
        CAPTURE(index);
        CAPTURE(indexed_addr);
        CAPTURE(scenario);

        state.registers.pc = pc_addr;
        memory.set_byte(pc_addr, base_addr);
        state.set_register(index_register, index);

        change_context context{indexed_addr, addr_modes[opcode], state, memory};
        apply_changes(scenario.initial_changes, context);

        expected_state = state;
        expected_memory = memory;

        expected_state.cycle = base_cycle_cost + scenario.base_cycle_cost;
        expected_state.registers.pc = pc_addr + 1;

        change_context expected_context{indexed_addr, addr_modes[opcode], expected_state, expected_memory};
        apply_changes(scenario.expected_changes, expected_context);

        execute_and_check(opcode);
    };

    DYNAMIC_SECTION(format("zero_page_{}", index_register))
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, index] = GENERATE(from_range(zero_page_indexed_scenarios));

            test(pc_addr, base_addr, index, addressing_scenario);
        }

        SECTION("behavior")
        {
            const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

            INFO(scenario.description);

            test(default_pc_addr, zero_page_base_addr, indexed_offset, scenario);
        }
    }
}

void execute_fixture::test_absolute(opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost)
{
    auto test = [this, opcode, base_cycle_cost](addr_x pc_addr, addr_x base_addr, const scenario& scenario) {
        CAPTURE(pc_addr);
        CAPTURE(base_addr);
        CAPTURE(scenario);

        state.registers.pc = pc_addr;
        memory.set_word(pc_addr, base_addr);

        change_context context{base_addr, addr_mode::absolute, state, memory};
        apply_changes(scenario.initial_changes, context);

        expected_state = state;
        expected_memory = memory;

        expected_state.cycle = base_cycle_cost + scenario.base_cycle_cost;
        expected_state.registers.pc = pc_addr + 2;

        change_context expected_context{base_addr, addr_mode::absolute, expected_state, expected_memory};
        apply_changes(scenario.expected_changes, expected_context);

        execute_and_check(opcode);
    };

    SECTION("absolute")
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr] = GENERATE(from_range(absolute_scenarios));

            test(pc_addr, base_addr, addressing_scenario);
        }

        SECTION("behavior")
        {
            const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

            INFO(scenario.description);

            test(default_pc_addr, absolute_base_addr, scenario);
        }
    }
}

void execute_fixture::test_absolute_indexed(opcode opcode, register_id index_register, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost)
{
    auto test = [this, opcode, index_register, base_cycle_cost](addr_x pc_addr, addr_x base_addr, byte_x index, const scenario& scenario) {
        const addr_x indexed_addr = base_addr + index;

        CAPTURE(pc_addr);
        CAPTURE(base_addr);
        CAPTURE(index_register);
        CAPTURE(index);
        CAPTURE(indexed_addr);
        CAPTURE(scenario);

        state.registers.pc = pc_addr;
        memory.set_word(pc_addr, base_addr);
        state.set_register(index_register, index);

        change_context context{indexed_addr, addr_modes[opcode], state, memory};
        apply_changes(scenario.initial_changes, context);

        expected_state = state;
        expected_memory = memory;

        expected_state.cycle = base_cycle_cost + scenario.base_cycle_cost;
        expected_state.registers.pc = pc_addr + 2;

        change_context expected_context{indexed_addr, addr_modes[opcode], expected_state, expected_memory};
        apply_changes(scenario.expected_changes, expected_context);

        execute_and_check(opcode, false);
    };

    DYNAMIC_SECTION(format("absolute_{}", index_register))
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, index] = GENERATE(from_range(absolute_indexed_scenarios));

            test(pc_addr, base_addr, index, addressing_scenario);
        }

        SECTION("behavior")
        {
            const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

            INFO(scenario.description);

            test(default_pc_addr, absolute_base_addr, indexed_offset, scenario);
        }
    }
}

void execute_fixture::test_indexed_indirect(opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost)
{
    auto test = [this, opcode, base_cycle_cost](addr_x pc_addr, byte_x base_addr, byte_x x, byte_x value_addr, const scenario& scenario) {
        CAPTURE(pc_addr);
        CAPTURE(base_addr);
        CAPTURE(x);
        CAPTURE(scenario);

        state.registers.pc = pc_addr;
        state.registers.x = x;
        memory.set_byte(pc_addr, base_addr);
        memory.set_byte((base_addr + x) & 0xFF, value_addr & 0xFF);
        memory.set_byte((base_addr + x + 1) & 0xFF, static_cast<byte_t>(value_addr >> 8));

        change_context context{value_addr, addr_modes[opcode], state, memory};
        apply_changes(scenario.initial_changes, context);

        expected_state = state;
        expected_memory = memory;

        expected_state.cycle = base_cycle_cost + scenario.base_cycle_cost;
        expected_state.registers.pc = pc_addr + 1;

        change_context expected_context{value_addr, addr_modes[opcode], expected_state, expected_memory};
        apply_changes(scenario.expected_changes, expected_context);

        execute_and_check(opcode);
    };

    SECTION("indexed_indirect")
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, value_addr, x] = GENERATE(from_range(indexed_indirect_scenarios));

            test(pc_addr, base_addr, x, value_addr, addressing_scenario);
        }

        SECTION("behavior")
        {
            const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

            INFO(scenario.description);

            test(default_pc_addr, default_base_addr, indexed_offset, 0xC0, scenario);
        }
    }
}

void execute_fixture::test_unspecified(opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost)
{
    SECTION("behavior")
    {
        const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

        INFO(scenario.description);

        CAPTURE(scenario);

        change_context context{default_pc_addr, addr_modes[opcode], state, memory};
        apply_changes(scenario.initial_changes, context);

        expected_state = state;
        expected_memory = memory;

        expected_state.cycle = base_cycle_cost + scenario.base_cycle_cost;

        change_context expected_context{default_pc_addr, addr_modes[opcode], expected_state, expected_memory};
        apply_changes(scenario.expected_changes, expected_context);

        execute_and_check(opcode);
    }
}

void execute_fixture::apply_changes(auto& changes, change_context& context)
{
    for (auto& change_variant : changes)
    {
        std::visit([&context](const auto& change) { change.apply(context); }, change_variant);
    }
}

void execute_fixture::execute_and_check(opcode code, bool should_check_cycle)
{
    REQUIRE(execute(execute_context(state, memory), code));

    check_registers();
    check_memory();

    if (should_check_cycle)
    {
        check_cycle();
    }
}

void execute_fixture::check_registers() const
{
    CHECK(state.registers.a == expected_state.registers.a);
    CHECK(state.registers.x == expected_state.registers.x);
    CHECK(state.registers.y == expected_state.registers.y);
    CHECK(state.registers.pc == expected_state.registers.pc);
    CHECK(state.registers.s == expected_state.registers.s);

    if (state.registers.p != expected_state.registers.p)
    {
        CHECK(state.registers.is_flag_set(status_flag::carry) == expected_state.registers.is_flag_set(status_flag::carry));
        CHECK(state.registers.is_flag_set(status_flag::zero) == expected_state.registers.is_flag_set(status_flag::zero));
        CHECK(state.registers.is_flag_set(status_flag::interrupt) == expected_state.registers.is_flag_set(status_flag::interrupt));
        CHECK(state.registers.is_flag_set(status_flag::decimal) == expected_state.registers.is_flag_set(status_flag::decimal));
        CHECK(state.registers.is_flag_set(status_flag::break_cmd) == expected_state.registers.is_flag_set(status_flag::break_cmd));
        CHECK(state.registers.is_flag_set(status_flag::unused) == expected_state.registers.is_flag_set(status_flag::unused));
        CHECK(state.registers.is_flag_set(status_flag::overflow) == expected_state.registers.is_flag_set(status_flag::overflow));
        CHECK(state.registers.is_flag_set(status_flag::negative) == expected_state.registers.is_flag_set(status_flag::negative));
    }
}

void execute_fixture::check_memory() const
{
    const auto& expected_memory_buffer = expected_memory.get_bytes();
    const auto& memory_buffer = memory.get_bytes();

    REQUIRE(expected_memory_buffer.size() == memory_buffer.size());

    if (std::memcmp(memory_buffer.data(), expected_memory_buffer.data(), memory_buffer.size()) != 0)
    {
        std::string fail_message = "expected_state.memory != state.memory\n";

        for (size_t addr = 0; addr < memory_buffer.size() - 1; ++addr)
        {
            if (memory_buffer[addr] != expected_memory_buffer[addr])
            {
                fmt::format_to(std::back_inserter(fail_message), "  Mismatch at address 0x{:04X}: expected 0x{:02X}, actual 0x{:02X}\n", static_cast<addr_t>(addr), expected_memory_buffer[addr], memory_buffer[addr]);
            }
        }

        FAIL_CHECK(fail_message.c_str());
    }
}

void execute_fixture::check_cycle() const
{
    CHECK(state.cycle == expected_state.cycle);
}

} // namespace nese::cpu::instruction
