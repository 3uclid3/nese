#pragma once

#include <array>

#include <magic_enum.hpp>

#include <nese/basic_types.hpp>
#include <nese/cpu.hpp>

namespace nese::v2 {

enum class cpu_register_id : u8_t
{
    a,
    x,
    y,

    pc,

    sp,

    status
};

constexpr auto format_as(cpu_register_id id)
{
    return magic_enum::enum_name(id);
}

struct cpu_bus_mock
{
    using memory_t = std::array<byte_t, 0x10000>;
    using cpu_t = cpu<cpu_bus_mock>;

    inline static memory_t default_memory{[] {
        byte_t value = 0xDE;
        byte_t alternate_value = 0xAD;

        memory_t memory;

        for (byte_t& byte : memory)
        {
            byte = value;

            std::swap(value, alternate_value);
        }

        return memory;
    }()};

    const auto& cpu_state() const { return cpu.get_state(); }
    auto& cpu_state() { return cpu.get_state({}); }

    const auto& cpu_registers() const { return cpu_state().registers; }
    auto& cpu_registers() { return cpu_state().registers; }

    cpu_bus_mock()
    {
        cpu_state().registers.a = 0xDE;
        cpu_state().registers.x = 0xDE;
        cpu_state().registers.y = 0xDE;
        cpu_state().registers.status = 0x0;
    }

    [[nodiscard]] constexpr byte_t read_byte(addr_t addr) const
    {
        return memory[addr];
    }

    [[nodiscard]] constexpr word_t read_word(addr_t addr) const
    {
        return read_byte(addr) + static_cast<word_t>(static_cast<word_t>(read_byte(addr + 1)) << 8);
    }

    constexpr void write_byte(addr_t addr, byte_t value)
    {
        memory[addr] = value;
    }

    constexpr void write_word(addr_t addr, word_t value)
    {
        write(addr, static_cast<byte_t>(value & 0xff));
        write(addr + 1, static_cast<byte_t>(value >> 8));
    }

    constexpr void write(addr_t addr, byte_t value)
    {
        write_byte(addr, value);
    }

    constexpr void write(addr_t addr, word_t value)
    {
        write_word(addr, value);
    }

    template<typename ValueT>
    void set_register_impl(cpu_register_id id, ValueT value)
    {
        switch (id)
        {
        case cpu_register_id::a:
            cpu_state().registers.a = static_cast<byte_t>(value);
            break;

        case cpu_register_id::x:
            cpu_state().registers.x = static_cast<byte_t>(value);
            break;

        case cpu_register_id::y:
            cpu_state().registers.y = static_cast<byte_t>(value);
            break;

        case cpu_register_id::sp:
            cpu_state().registers.sp = static_cast<byte_t>(value);
            break;

        case cpu_register_id::status:
            cpu_state().registers.status = static_cast<byte_t>(value);
            break;

        case cpu_register_id::pc:
            cpu_state().registers.pc = static_cast<word_t>(value);
            break;
        }
    }

    void set_register(cpu_register_id id, byte_t value)
    {
        set_register_impl(id, value);
    }

    void set_register(cpu_register_id id, word_t value)
    {
        set_register_impl(id, value);
    }

    memory_t memory{default_memory};
    cpu_t cpu{{}, *this};
};

} // namespace nese::v2
