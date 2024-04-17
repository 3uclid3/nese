#include <nese/cpu_fixture.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <array>
#include <string>

#include <nese/cpu.hpp>
#include <nese/cpu/stack.hpp>
#include <nese/utility/hex.hpp>

namespace nese::v2 {

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
        ctx.bus.write(ctx.operand_addr, value);
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
        ctx.bus.write(addr, value);
    }
    else if constexpr (std::is_same_v<ValueT, word_t>)
    {
        ctx.bus.write(addr, value);
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

        ctx.bus.write_byte(cpu_stack_offset + sp0, value & 0xFF);
        ctx.bus.write_byte(cpu_stack_offset + sp1, value >> 8);
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
        ctx.bus.write_byte(registers.sp + cpu_stack_offset, value & 0xFF);
        --registers.sp;

        ctx.bus.write_byte(registers.sp + cpu_stack_offset, value >> 8);
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

cpu_fixture::op::set_operand<word_t> cpu_fixture::set_operand(word_t value)
{
    return {value};
}

cpu_fixture::op::set_memory<byte_t> cpu_fixture::set_memory(addr_t addr, byte_t value)
{
    return {addr, value};
}

cpu_fixture::op::set_memory<word_t> cpu_fixture::set_memory(addr_t addr, word_t value)
{
    return {addr, value};
}

cpu_fixture::op::set_stack<byte_t> cpu_fixture::set_stack(byte_t sp, byte_t value)
{
    return {sp, value};
}

cpu_fixture::op::set_stack<word_t> cpu_fixture::set_stack(byte_t sp, word_t value)
{
    return {sp, value};
}

cpu_fixture::op::push_stack<byte_t> cpu_fixture::push_stack(byte_t value)
{
    return {value};
}

cpu_fixture::op::push_stack<word_t> cpu_fixture::push_stack(word_t value)
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