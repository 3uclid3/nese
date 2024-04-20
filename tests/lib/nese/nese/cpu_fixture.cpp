#include <nese/cpu_fixture.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <array>
#include <string>

#include <nese/cpu.hpp>
#include <nese/cpu/cpu_stack_offset.hpp>
#include <nese/utility/hex.hpp>

namespace nese::v2 {

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
    {// TODO Meaningful values
     // [pc_addr, base_addr, x, value_addr]
     {0x0200, 0x01, 0x01, 0x11},
     {0x0200, 0x80, 0x80, 0x10},
     {0x0000, 0x10, 0x2, 0x30},
     {0x8000, 0x03, 0x3, 0x40},
     {0xFFF9, 0x04, 0x4, 0x50}});

static constexpr std::array indirect_indexed_scenarios = std::to_array<std::tuple<addr_x, byte_x, byte_x, byte_x>>(
    {// TODO Meaningful values
     // [pc_addr, base_addr, x, value_addr]
     {0x0200, 0x01, 0x01, 0x11},
     {0x0200, 0x80, 0x80, 0x10},
     {0x0000, 0x10, 0x2, 0x30},
     {0x8000, 0x03, 0x3, 0x40},
     {0xFFF9, 0x04, 0x4, 0x50}});

template<cpu_memval ValueT>
cpu_fixture::op::set_operand<ValueT>::set_operand(ValueT v)
    : value(v)
{
}

template<cpu_memval ValueT>
void cpu_fixture::op::set_operand<ValueT>::execute(operation_context& ctx) const
{
    switch (ctx.addr_mode)
    {
    case cpu_addr_mode::accumulator:
        ctx.bus.cpu_state().registers.a = static_cast<byte_t>(value);
        break;

    default:
        ctx.bus.write_any(ctx.operand_addr, value);
        break;
    }
}

template<cpu_memval ValueT>
string_view cpu_fixture::op::set_operand<ValueT>::to_string() const
{
    return format("operand value := {}", value);
}

template<cpu_memval ValueT>
cpu_fixture::op::set_memory<ValueT>::set_memory(addr_t a, ValueT v)
    : addr(a)
    , value(v)
{
}

template<cpu_memval ValueT>
void cpu_fixture::op::set_memory<ValueT>::execute(operation_context& ctx) const
{
    if constexpr (std::is_same_v<ValueT, byte_t>)
    {
        ctx.bus.write_any(addr, value);
    }
    else if constexpr (std::is_same_v<ValueT, word_t>)
    {
        ctx.bus.write_any(addr, value);
    }
}

template<cpu_memval ValueT>
string_view cpu_fixture::op::set_memory<ValueT>::to_string() const
{
    return format("{} := {}", addr, value);
}

template<cpu_memval ValueT>
cpu_fixture::op::set_stack<ValueT>::set_stack(byte_t sp_, ValueT value_)
    : sp(sp_)
    , value(value_)
{
}

template<cpu_memval ValueT>
void cpu_fixture::op::set_stack<ValueT>::execute(operation_context& ctx) const
{
    if constexpr (std::is_same_v<ValueT, byte_t>)
    {
        ctx.bus.write(cpu_stack_offset + sp, value);
    }
    else if constexpr (std::is_same_v<ValueT, word_t>)
    {
        const byte_t sp0 = sp;
        const byte_t sp1 = sp + 1;

        ctx.bus.write(cpu_stack_offset + sp0, value & 0xFF);
        ctx.bus.write(cpu_stack_offset + sp1, value >> 8);
    }
}

template<cpu_memval ValueT>
string_view cpu_fixture::op::set_stack<ValueT>::to_string() const
{
    return format("sp := {}, value := {}", sp, value);
}

template<cpu_memval ValueT>
cpu_fixture::op::push_stack<ValueT>::push_stack(ValueT value_)
    : value(value_)
{
}

template<cpu_memval ValueT>
void cpu_fixture::op::push_stack<ValueT>::execute(operation_context& ctx) const
{
    auto& registers = ctx.bus.cpu_state().registers;

    if constexpr (std::is_same_v<ValueT, byte_t>)
    {
        ctx.bus.write(registers.sp + cpu_stack_offset, value);
        --registers.sp;
    }
    else if constexpr (std::is_same_v<ValueT, word_t>)
    {
        ctx.bus.write(registers.sp + cpu_stack_offset, value & 0xFF);
        --registers.sp;

        ctx.bus.write(registers.sp + cpu_stack_offset, value >> 8);
        --registers.sp;
    }
}

template<cpu_memval ValueT>
string_view cpu_fixture::op::push_stack<ValueT>::to_string() const
{
    return format("push value := {}", value);
}

template<cpu_register_id RegisterT, cpu_memval ValueT>
cpu_fixture::op::set_register<RegisterT, ValueT>::set_register(ValueT v)
    : value(v)
{}

template<cpu_register_id RegisterT, cpu_memval ValueT>
void cpu_fixture::op::set_register<RegisterT, ValueT>::execute(operation_context& ctx) const
{
    ctx.bus.set_register(RegisterT, value);
}

template<cpu_register_id RegisterT, cpu_memval ValueT>
string_view cpu_fixture::op::set_register<RegisterT, ValueT>::to_string() const
{
    return format("{} := {}", RegisterT, value);
}

template<cpu_status StatusFlagT>
void cpu_fixture::op::set_register_status<StatusFlagT>::execute(operation_context& ctx) const
{
    ctx.bus.cpu_state().registers.set_status(StatusFlagT);
}

template<cpu_status StatusFlagT>
string_view cpu_fixture::op::set_register_status<StatusFlagT>::to_string() const
{
    return format("{} := true", StatusFlagT);
}

template<cpu_status StatusFlagT>
void cpu_fixture::op::clear_register_status<StatusFlagT>::execute(operation_context& ctx) const
{
    ctx.bus.cpu_state().registers.clear_status(StatusFlagT);
}

template<cpu_status StatusFlagT>
string_view cpu_fixture::op::clear_register_status<StatusFlagT>::to_string() const
{
    return format("{} := false", StatusFlagT);
}

template<typename BasicOperationT>
cpu_fixture::op::conditional<BasicOperationT>::conditional(BasicOperationT op, bool condition)
    : operation(op)
    , condition(condition)
{}

template<typename BasicOperationT>
void cpu_fixture::op::conditional<BasicOperationT>::execute(operation_context& ctx) const
{
    if (condition)
    {
        std::visit([&ctx](const auto& op) { op.execute(ctx); }, operation);
    }
}

template<typename BasicOperationT>
string_view cpu_fixture::op::conditional<BasicOperationT>::to_string() const
{
    string_view result{};

    if (condition)
    {
        std::visit([&result](const auto& op) { result = op.to_string(); }, operation);
    }

    return result;
}

string cpu_fixture::scenario::to_string() const
{
    auto append_operation_strings = [](auto& string, const auto& operations) {
        for (const auto& operation_variant : operations)
        {
            string_view operation_string;
            std::visit([&operation_string](const auto& operation) { operation_string = operation.to_string(); }, operation_variant);

            if (!operation_string.empty())
            {
                string += "\n    ";
                string += operation_string;
            }
        }
    };

    string string;
    string.reserve(1024);

    string += "\n  ";
    string += "initial := [";
    append_operation_strings(string, initial);
    string += "\n  ]";

    string += "\n  ";
    string += "expected := [";
    append_operation_strings(string, expected);
    string += "\n  ]";

    return string;
}

cpu_fixture::op::set_operand<byte_t> cpu_fixture::set_operand(byte_t value)
{
    return {value};
}

cpu_fixture::op::set_operand<word_t> cpu_fixture::set_operandw(word_t value)
{
    return {value};
}

cpu_fixture::op::set_memory<byte_t> cpu_fixture::set_memory(addr_t addr, byte_t value)
{
    return {addr, value};
}

cpu_fixture::op::set_memory<word_t> cpu_fixture::set_memoryw(addr_t addr, word_t value)
{
    return {addr, value};
}

cpu_fixture::op::set_stack<byte_t> cpu_fixture::set_stack(byte_t sp, byte_t value)
{
    return {sp, value};
}

cpu_fixture::op::set_stack<word_t> cpu_fixture::set_stackw(byte_t sp, word_t value)
{
    return {sp, value};
}

cpu_fixture::op::push_stack<byte_t> cpu_fixture::push_stack(byte_t value)
{
    return {value};
}

cpu_fixture::op::push_stack<word_t> cpu_fixture::push_stackw(word_t value)
{
    return {value};
}

cpu_fixture::op::set_register<cpu_register_id::a, byte_t> cpu_fixture::set_a(byte_t value)
{
    return {value};
}

cpu_fixture::op::set_register<cpu_register_id::x, byte_t> cpu_fixture::set_x(byte_t value)
{
    return {value};
}

cpu_fixture::op::set_register<cpu_register_id::y, byte_t> cpu_fixture::set_y(byte_t value)
{
    return {value};
}

cpu_fixture::op::set_register<cpu_register_id::sp, byte_t> cpu_fixture::set_sp(byte_t value)
{
    return {value};
}

cpu_fixture::op::set_register<cpu_register_id::pc, word_t> cpu_fixture::set_pc(word_t value)
{
    return {value};
}

cpu_fixture::op::set_register<cpu_register_id::status, byte_t> cpu_fixture::set_status(byte_t value)
{
    return {value};
}

cpu_fixture::op::set_register<cpu_register_id::status, byte_t> cpu_fixture::set_status(cpu_status value)
{
    return {static_cast<byte_t>(value)};
}

cpu_fixture::op::set_register_status<cpu_status::carry> cpu_fixture::set_status_carry()
{
    return {};
}

cpu_fixture::op::set_register_status<cpu_status::break_cmd> cpu_fixture::set_status_break_cmd()
{
    return {};
}

cpu_fixture::op::set_register_status<cpu_status::decimal> cpu_fixture::set_status_decimal()
{
    return {};
}

cpu_fixture::op::set_register_status<cpu_status::interrupt> cpu_fixture::set_status_interrupt()
{
    return {};
}

cpu_fixture::op::set_register_status<cpu_status::negative> cpu_fixture::set_status_negative()
{
    return {};
}

cpu_fixture::op::set_register_status<cpu_status::overflow> cpu_fixture::set_status_overflow()
{
    return {};
}

cpu_fixture::op::set_register_status<cpu_status::unused> cpu_fixture::set_status_unused()
{
    return {};
}

cpu_fixture::op::set_register_status<cpu_status::zero> cpu_fixture::set_status_zero()
{
    return {};
}

cpu_fixture::op::clear_register_status<cpu_status::carry> cpu_fixture::clear_status_carry()
{
    return {};
}

cpu_fixture::op::clear_register_status<cpu_status::break_cmd> cpu_fixture::clear_status_break_cmd()
{
    return {};
}

cpu_fixture::op::clear_register_status<cpu_status::decimal> cpu_fixture::clear_status_decimal()
{
    return {};
}

cpu_fixture::op::clear_register_status<cpu_status::interrupt> cpu_fixture::clear_status_interrupt()
{
    return {};
}

cpu_fixture::op::clear_register_status<cpu_status::negative> cpu_fixture::clear_status_negative()
{
    return {};
}

cpu_fixture::op::clear_register_status<cpu_status::overflow> cpu_fixture::clear_status_overflow()
{
    return {};
}

cpu_fixture::op::clear_register_status<cpu_status::unused> cpu_fixture::clear_status_unused()
{
    return {};
}

cpu_fixture::op::clear_register_status<cpu_status::zero> cpu_fixture::clear_status_zero()
{
    return {};
}

cpu_fixture::op::conditional<cpu_fixture::basic_operation> cpu_fixture::conditional(basic_operation operation, bool condition)
{
    return {operation, condition};
}

void cpu_fixture::execute_operations(auto& operations, operation_context& context)
{
    for (auto& operation_variant : operations)
    {
        std::visit([&context](const auto& operation) { operation.execute(context); }, operation_variant);
    }
}

void cpu_fixture::test_implied(cpu_opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost)
{
    SECTION("implied")
    {
        const auto& scenario = GENERATE_COPY(from_range(behavior_scenarios));

        INFO(scenario.description);
        CAPTURE(scenario);

        if (scenario.debug_break)
        {
            __debugbreak();
        }

        bus.cpu_state().registers.pc = default_pc_addr;

        operation_context context{default_pc_addr, cpu_addr_mode::implied, bus};
        execute_operations(scenario.initial, context);

        expected_bus = bus;
        expected_bus.cpu_state().cycle = cycle_cost + scenario.cycle_cost;

        operation_context expected_context{default_pc_addr, cpu_addr_mode::implied, expected_bus};
        execute_operations(scenario.expected, expected_context);

        execute_and_check(opcode);
    }
}

void cpu_fixture::test_immediate(cpu_opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost)
{
    auto test = [this, opcode, cycle_cost](addr_x pc_addr, const scenario& scenario) {
        CAPTURE(scenario);

        if (scenario.debug_break)
        {
            __debugbreak();
        }

        bus.cpu_state().registers.pc = pc_addr;

        operation_context context{pc_addr, cpu_addr_mode::immediate, bus};
        execute_operations(scenario.initial, context);

        expected_bus = bus;

        expected_bus.cpu_state().cycle = cycle_cost + scenario.cycle_cost;
        expected_bus.cpu_state().registers.pc = pc_addr + 1;

        operation_context expected_context{pc_addr, cpu_addr_mode::immediate, expected_bus};
        execute_operations(scenario.expected, expected_context);

        execute_and_check(opcode);
    };

    SECTION("immediate")
    {
        SECTION("addressing")
        {
            const addr_x pc_addr = GENERATE(as<addr_x>(), 0x0000, 0x7FFE, 0xFFFE);

            CAPTURE(pc_addr);

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

void cpu_fixture::test_acculumator(cpu_opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost)
{
    SECTION("accumulator")
    {
        const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

        INFO(scenario.description);
        CAPTURE(scenario);

        if (scenario.debug_break)
        {
            __debugbreak();
        }

        operation_context context{default_pc_addr, cpu_addr_mode::accumulator, bus};
        execute_operations(scenario.initial, context);

        expected_bus = bus;

        expected_bus.cpu_state().cycle = cycle_cost + scenario.cycle_cost;

        operation_context expected_context{default_pc_addr, cpu_addr_mode::accumulator, expected_bus};
        execute_operations(scenario.expected, expected_context);

        execute_and_check(opcode);
    }
}

void cpu_fixture::test_zero_page(cpu_opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost)
{
    auto test = [this, opcode, cycle_cost](addr_t pc_addr, byte_t base_addr, const scenario& scenario) {
        CAPTURE(scenario);

        if (scenario.debug_break)
        {
            __debugbreak();
        }

        bus.cpu_state().registers.pc = pc_addr;
        bus.write(pc_addr, base_addr);

        operation_context context{base_addr, cpu_addr_mode::zero_page, bus};
        execute_operations(scenario.initial, context);

        expected_bus = bus;

        expected_bus.cpu_state().cycle = cycle_cost + scenario.cycle_cost;
        expected_bus.cpu_state().registers.pc = pc_addr + 1;

        operation_context expected_context{base_addr, cpu_addr_mode::zero_page, expected_bus};
        execute_operations(scenario.expected, expected_context);

        execute_and_check(opcode);
    };

    SECTION("zero_page")
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr] = GENERATE(from_range(zero_page_scenarios));

            CAPTURE(pc_addr);
            CAPTURE(base_addr);

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

void cpu_fixture::test_zero_page_indexed(cpu_opcode opcode, cpu_register_id index_register, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost)
{
    auto test = [this, opcode, index_register, cycle_cost](addr_t pc_addr, byte_t base_addr, byte_t index, const scenario& scenario) {
        CAPTURE(scenario);

        if (scenario.debug_break)
        {
            __debugbreak();
        }

        const byte_t indexed_addr = base_addr + index & 0xff;
        const cpu_addr_mode addr_mode = cpu_opcode_addr_modes[opcode];

        bus.cpu_state().registers.pc = pc_addr;
        bus.write(pc_addr, base_addr);
        bus.set_register(index_register, index);

        operation_context context{indexed_addr, addr_mode, bus};
        execute_operations(scenario.initial, context);

        expected_bus = bus;

        expected_bus.cpu_state().cycle = cycle_cost + scenario.cycle_cost;
        expected_bus.cpu_state().registers.pc = pc_addr + 1;

        operation_context expected_context{indexed_addr, addr_mode, expected_bus};
        execute_operations(scenario.expected, expected_context);

        execute_and_check(opcode);
    };

    DYNAMIC_SECTION(format("zero_page_{}", index_register))
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, index] = GENERATE(from_range(zero_page_indexed_scenarios));

            CAPTURE(pc_addr);
            CAPTURE(base_addr);
            CAPTURE(index_register);
            CAPTURE(index);

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

void cpu_fixture::test_absolute(cpu_opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost)
{
    auto test = [this, opcode, cycle_cost](addr_t pc_addr, addr_t base_addr, const scenario& scenario) {
        CAPTURE(scenario);

        if (scenario.debug_break)
        {
            __debugbreak();
        }

        bus.cpu_state().registers.pc = pc_addr;
        bus.write_word(pc_addr, base_addr);

        operation_context context{base_addr, cpu_addr_mode::absolute, bus};
        execute_operations(scenario.initial, context);

        expected_bus = bus;

        expected_bus.cpu_state().cycle = cycle_cost + scenario.cycle_cost;
        expected_bus.cpu_state().registers.pc = pc_addr + 2;

        operation_context expected_context{base_addr, cpu_addr_mode::absolute, expected_bus};
        execute_operations(scenario.expected, expected_context);

        execute_and_check(opcode);
    };

    SECTION("absolute")
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr] = GENERATE(from_range(absolute_scenarios));

            CAPTURE(pc_addr);
            CAPTURE(base_addr);

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

void cpu_fixture::test_absolute_indexed(cpu_opcode opcode, cpu_register_id index_register, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost)
{
    auto test = [this, opcode, index_register, cycle_cost](addr_x pc_addr, addr_x base_addr, byte_x index, const scenario& scenario) {
        CAPTURE(scenario);

        if (scenario.debug_break)
        {
            __debugbreak();
        }

        const addr_x indexed_addr = base_addr + index;
        const cpu_addr_mode addr_mode = cpu_opcode_addr_modes[opcode];

        bus.cpu_state().registers.pc = pc_addr;
        bus.write_word(pc_addr, base_addr);
        bus.set_register(index_register, index);

        operation_context context{indexed_addr, addr_mode, bus};
        execute_operations(scenario.initial, context);

        expected_bus = bus;

        expected_bus.cpu_state().cycle = cycle_cost + scenario.cycle_cost;
        expected_bus.cpu_state().registers.pc = pc_addr + 2;

        operation_context expected_context{indexed_addr, addr_mode, expected_bus};
        execute_operations(scenario.expected, expected_context);

        execute_and_check(opcode, false);
    };

    DYNAMIC_SECTION(format("absolute_{}", index_register))
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, index] = GENERATE(from_range(absolute_indexed_scenarios));

            CAPTURE(pc_addr);
            CAPTURE(base_addr);
            CAPTURE(index);

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

void cpu_fixture::test_indexed_indirect(cpu_opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost)
{
    auto test = [this, opcode, cycle_cost](addr_x pc_addr, byte_x base_addr, byte_x x, byte_x value_addr, const scenario& scenario) {
        CAPTURE(scenario);

        if (scenario.debug_break)
        {
            __debugbreak();
        }

        bus.cpu_state().registers.pc = pc_addr;
        bus.cpu_state().registers.x = x;
        bus.write(pc_addr, base_addr);
        bus.write((base_addr + x) & 0xFF, value_addr & 0xFF);
        bus.write((base_addr + x + 1) & 0xFF, static_cast<byte_t>(value_addr >> 8));

        operation_context context{value_addr, cpu_addr_mode::indexed_indirect, bus};
        execute_operations(scenario.initial, context);

        expected_bus = bus;

        expected_bus.cpu_state().cycle = cycle_cost + scenario.cycle_cost;
        expected_bus.cpu_state().registers.pc = pc_addr + 1;

        operation_context expected_context{value_addr, cpu_addr_mode::indexed_indirect, expected_bus};
        execute_operations(scenario.expected, expected_context);

        execute_and_check(opcode);
    };

    SECTION("indexed_indirect")
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, value_addr, x] = GENERATE(from_range(indexed_indirect_scenarios));

            CAPTURE(pc_addr);
            CAPTURE(base_addr);
            CAPTURE(x);

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

void cpu_fixture::test_indirect_indexed(cpu_opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost)
{
    auto test = [this, opcode, cycle_cost](addr_x pc_addr, byte_x base_addr, byte_x y, byte_x value_addr, const scenario& scenario) {
        CAPTURE(scenario);

        if (scenario.debug_break)
        {
            __debugbreak();
        }

        bus.cpu_state().registers.pc = pc_addr;
        bus.cpu_state().registers.y = y;
        bus.write(pc_addr, base_addr);
        bus.write(base_addr, value_addr & 0xFF);
        bus.write((base_addr + 1) & 0xFF, static_cast<byte_t>(value_addr >> 8));

        operation_context context{static_cast<addr_t>(value_addr + y), cpu_addr_mode::indirect_indexed, bus};
        execute_operations(scenario.initial, context);

        expected_bus = bus;

        expected_bus.cpu_state().cycle = cycle_cost + scenario.cycle_cost;
        expected_bus.cpu_state().registers.pc = pc_addr + 1;

        operation_context expected_context{static_cast<addr_t>(value_addr + y), cpu_addr_mode::indirect_indexed, expected_bus};
        execute_operations(scenario.expected, expected_context);

        execute_and_check(opcode, false);
    };

    SECTION("indirect_indexed")
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, value_addr, y] = GENERATE(from_range(indirect_indexed_scenarios));

            CAPTURE(pc_addr);
            CAPTURE(base_addr);
            CAPTURE(y);

            test(pc_addr, base_addr, y, value_addr, addressing_scenario);
        }

        SECTION("behavior")
        {
            const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

            INFO(scenario.description);

            test(default_pc_addr, default_base_addr, indexed_offset, 0xC0, scenario);
        }
    }
}

void cpu_fixture::test_relative(cpu_opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost)
{
    SECTION("relative")
    {
        const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

        INFO(scenario.description);
        CAPTURE(scenario);

        if (scenario.debug_break)
        {
            __debugbreak();
        }

        operation_context context{default_pc_addr, cpu_addr_mode::relative, bus};
        execute_operations(scenario.initial, context);

        expected_bus = bus;

        expected_bus.cpu_state().cycle = cycle_cost + scenario.cycle_cost;

        operation_context expected_context{default_pc_addr, cpu_addr_mode::relative, expected_bus};
        execute_operations(scenario.expected, expected_context);

        execute_and_check(opcode);
    }
}

void cpu_fixture::test_unspecified(cpu_opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost)
{
    const scenario& scenario = GENERATE_COPY(from_range(behavior_scenarios));

    INFO(scenario.description);
    CAPTURE(scenario);

    if (scenario.debug_break)
    {
        __debugbreak();
    }

    operation_context context{default_pc_addr, cpu_opcode_addr_modes[opcode], bus};
    execute_operations(scenario.initial, context);

    expected_bus = bus;

    expected_bus.cpu_state().cycle = cycle_cost + scenario.cycle_cost;

    operation_context expected_context{default_pc_addr, cpu_opcode_addr_modes[opcode], expected_bus};
    execute_operations(scenario.expected, expected_context);

    execute_and_check(opcode);
}

void cpu_fixture::execute_and_check(cpu_opcode opcode, bool should_check_cycle)
{
    bus.memory[--bus.cpu_state().registers.pc] = static_cast<byte_t>(opcode);

    REQUIRE(bus.cpu.step());

    check_registers();
    check_memory();

    if (should_check_cycle)
        check_cycle();
}

void cpu_fixture::check_registers() const
{
    const auto& registers = bus.cpu.get_state().registers;
    const auto& expected_registers = expected_bus.cpu.get_state().registers;

    const byte_x a = registers.a;
    const byte_x expected_a = expected_registers.a;
    CHECK(a == expected_a);

    const byte_x x = registers.x;
    const byte_x expected_x = expected_registers.x;
    CHECK(x == expected_x);

    const byte_x y = registers.y;
    const byte_x expected_y = expected_registers.y;
    CHECK(y == expected_y);

    const word_x pc = registers.pc;
    const word_x expected_pc = expected_registers.pc;
    CHECK(pc == expected_pc);

    const byte_x sp = registers.sp;
    const byte_x expected_sp = expected_registers.sp;
    CHECK(sp == expected_sp);

    if (registers.status != expected_registers.status)
    {
        CHECK(registers.is_status_set(cpu_status::carry) == expected_registers.is_status_set(cpu_status::carry));
        CHECK(registers.is_status_set(cpu_status::zero) == expected_registers.is_status_set(cpu_status::zero));
        CHECK(registers.is_status_set(cpu_status::interrupt) == expected_registers.is_status_set(cpu_status::interrupt));
        CHECK(registers.is_status_set(cpu_status::decimal) == expected_registers.is_status_set(cpu_status::decimal));
        CHECK(registers.is_status_set(cpu_status::break_cmd) == expected_registers.is_status_set(cpu_status::break_cmd));
        CHECK(registers.is_status_set(cpu_status::unused) == expected_registers.is_status_set(cpu_status::unused));
        CHECK(registers.is_status_set(cpu_status::overflow) == expected_registers.is_status_set(cpu_status::overflow));
        CHECK(registers.is_status_set(cpu_status::negative) == expected_registers.is_status_set(cpu_status::negative));
    }
}

void cpu_fixture::check_memory() const
{
    const auto& expected_memory_buffer = expected_bus.memory;
    const auto& memory_buffer = expected_bus.memory;

    REQUIRE(expected_memory_buffer.size() == memory_buffer.size());

    if (std::memcmp(memory_buffer.data(), expected_memory_buffer.data(), memory_buffer.size()) != 0)
    {
        std::string fail_message = "memory != expected_memory\n";

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

void cpu_fixture::check_cycle() const
{
    const hex<u64_t> cycle = bus.cpu.get_state().cycle.count();
    const hex<u64_t> expected_cycle = expected_bus.cpu.get_state().cycle.count();

    CHECK(cycle == expected_cycle);
}

} // namespace nese::v2