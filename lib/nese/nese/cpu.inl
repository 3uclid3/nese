#include <magic_enum.hpp>

template<>
struct magic_enum::customize::enum_range<nese::v2::cpu_addr_mode>
{
    static constexpr bool is_flags = false;

    static constexpr int min = 0;
    static constexpr int max = static_cast<int>(nese::v2::cpu_addr_mode::count);
};

template<>
struct magic_enum::customize::enum_range<nese::v2::cpu_status>
{
    static constexpr bool is_flags = true;
};

namespace nese::v2 {

constexpr auto format_as(cpu_opcode opcode)
{
    return static_cast<std::underlying_type_t<cpu_opcode>>(opcode);
}

constexpr auto format_as(cpu_status status)
{
    return magic_enum::enum_name(status);
}

[[nodiscard]] constexpr cpu_status operator|(cpu_status lhs, cpu_status rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<cpu_status>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr cpu_status operator|(u8_t lhs, cpu_status rhs) noexcept
{
    return static_cast<cpu_status>(lhs) | rhs;
}

[[nodiscard]] constexpr cpu_status operator&(cpu_status lhs, cpu_status rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<cpu_status>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr cpu_status operator&(u8_t lhs, cpu_status rhs) noexcept
{
    return static_cast<cpu_status>(lhs) & rhs;
}

[[nodiscard]] constexpr cpu_status operator~(cpu_status w) noexcept
{
    // every static_cast is intentional
    return static_cast<cpu_status>(static_cast<u8_t>(~static_cast<unsigned int>(w)));
}

constexpr bool cpu_registers::is_status_set(cpu_status flag) const
{
    return (status & flag) != cpu_status::none;
}

constexpr bool cpu_registers::is_status_clear(cpu_status flag) const
{
    return !is_status_set(flag);
}

constexpr void cpu_registers::set_status(cpu_status flag, bool value)
{
    status = static_cast<byte_t>(value ? status | flag : status & ~flag);
}

constexpr void cpu_registers::set_status(cpu_status flag)
{
    status = static_cast<byte_t>(status | flag);
}

constexpr void cpu_registers::clear_status(cpu_status flag)
{
    status = static_cast<byte_t>(status & ~flag);
}

template<typename BusT>
constexpr const typename cpu<BusT>::instruction& cpu<BusT>::instruction_table::operator[](cpu_opcode op) const
{
    return instructions[static_cast<size_t>(op)];
}

template<typename BusT>
constexpr typename cpu<BusT>::instruction& cpu<BusT>::instruction_table::operator[](cpu_opcode op)
{
    return instructions[static_cast<size_t>(op)];
}

template<typename BusT>
cpu<BusT>::cpu(passkey<BusT>, BusT& bus)
    : _bus(bus)
{
}

template<typename BusT>
void cpu<BusT>::reset()
{
}

template<typename BusT>
void cpu<BusT>::irq()
{
}

template<typename BusT>
void cpu<BusT>::nmi()
{
}

template<typename BusT>
bool cpu<BusT>::step()
{
    const auto opcode = static_cast<cpu_opcode>(decode());
    const auto instruction = _instruction_table[opcode];

    if (instruction == nullptr) [[unlikely]]
    {
        NESE_ERROR("[cpu] Unimplemented instruction {:02X}", opcode);
        return false;
    }

    ((*this).*instruction)();

    return true;
}

template<typename BusT>
const cpu_state& cpu<BusT>::get_state() const
{
    return _state;
}

template<typename BusT>
cpu_state& cpu<BusT>::get_state(passkey<BusT>)
{
    return _state;
}

template<typename BusT>
consteval typename cpu<BusT>::instruction_table cpu<BusT>::create_instruction_table()
{
    instruction_table instructions{};

    instructions[cpu_opcode::nop_implied] = &cpu::instruction_nop<cpu_addr_mode::implied>;

    return instructions;
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_nop()
{
    if constexpr (AddrModeT != cpu_addr_mode::implied)
    {
        [[maybe_unused]] const word_t operand = decode_operand<AddrModeT>();

        _state.cycle += get_addr_mode_cycle_cost<AddrModeT>();
    }
    else
    {
        _state.cycle += cpu_cycle_t(2);
    }
}
template<typename BusT>
byte_t cpu<BusT>::read(addr_t addr)
{
    return _bus.get().read_byte(addr);
}

template<typename BusT>
word_t cpu<BusT>::readw(addr_t addr)
{
    return _bus.get().read_word(addr);
}

template<typename BusT>
void cpu<BusT>::write(addr_t addr, byte_t byte)
{
    return _bus.get().write_byte(addr, byte);
}

template<typename BusT>
void cpu<BusT>::writew(addr_t addr, word_t word)
{
    return _bus.get().write_word(addr, word);
}

template<typename BusT>
byte_t cpu<BusT>::decode()
{
    return read(_state.registers.pc++);
}

template<typename BusT>
word_t cpu<BusT>::decodew()
{
    return readw(_state.registers.pc++);
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
constexpr cpu_cycle_t cpu<BusT>::get_addr_mode_cycle_cost(bool page_crossing)
{
    switch (AddrModeT)
    {
    case cpu_addr_mode::accumulator:
    case cpu_addr_mode::immediate:
        return cpu_cycle_t(2);

    case cpu_addr_mode::zero_page:
        return cpu_cycle_t(3);

    case cpu_addr_mode::zero_page_x:
    case cpu_addr_mode::zero_page_y:
    case cpu_addr_mode::absolute:
        return cpu_cycle_t(4);

    case cpu_addr_mode::absolute_x:
    case cpu_addr_mode::absolute_y:
        return cpu_cycle_t(page_crossing ? 5 : 4);

    case cpu_addr_mode::indexed_indirect:
        return cpu_cycle_t(6);

    case cpu_addr_mode::indirect_indexed:
        return cpu_cycle_t(page_crossing ? 6 : 5);

    default:
        NESE_ASSERT(false);
        return cpu_cycle_t(0);
    }
}

}