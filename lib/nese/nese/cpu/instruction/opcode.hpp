#pragma once

#include <catch2/catch_message.hpp>
#include <magic_enum.hpp>

#include <nese/basic_types.hpp>
#include <nese/cpu/addr_mode.hpp>
#include <nese/cpu/instruction/opcode_enum.hpp>

template<>
struct magic_enum::customize::enum_range<nese::cpu::instruction::opcode>
{
    static constexpr bool is_flags = false;

    static constexpr int min = 0x00;
    static constexpr int max = 0xFF;
};

namespace nese::cpu::instruction {

class mnemonic_table
{
public:
    static consteval mnemonic_table create()
    {
        mnemonic_table table;

        for (size_t i = 0; i < table._mnemonics.size(); ++i)
        {
            const string_view name = magic_enum::enum_name(static_cast<opcode>(i));

            if (name.empty())
                continue;

            const size_t index = name.find_first_of('_');

            table._mnemonics[i] = name.substr(0, index);
        }

        return table;
    }

    constexpr string_view operator[](size_t opcode) const
    {
        return _mnemonics[opcode];
    }

    constexpr string_view operator[](opcode opcode) const
    {
        return _mnemonics[static_cast<size_t>(opcode)];
    }

private:
    std::array<string_view, 256> _mnemonics{};
};

class addr_mode_table
{
public:
    static consteval addr_mode_table create()
    {
        addr_mode_table table;

        for (size_t i = 0; i < table._addr_modes.size(); ++i)
        {
            const string_view name = magic_enum::enum_name(static_cast<opcode>(i));

            if (name.empty())
                continue;

            const size_t index = name.find_first_of('_');
            const string_view addr_mode = name.substr(index + 1); // skip the '_'

            // clang-format off
            if (addr_mode == "implied") table._addr_modes[i] = addr_mode::implied;
            else if (addr_mode == "accumulator") table._addr_modes[i] = addr_mode::accumulator;
            else if (addr_mode == "immediate") table._addr_modes[i] = addr_mode::immediate;
            else if (addr_mode == "zero_page_x") table._addr_modes[i] = addr_mode::zero_page_x;
            else if (addr_mode == "zero_page_y") table._addr_modes[i] = addr_mode::zero_page_y;
            else if (addr_mode == "zero_page") table._addr_modes[i] = addr_mode::zero_page;
            else if (addr_mode == "absolute_x") table._addr_modes[i] = addr_mode::absolute_x;
            else if (addr_mode == "absolute_y") table._addr_modes[i] = addr_mode::absolute_y;
            else if (addr_mode == "absolute") table._addr_modes[i] = addr_mode::absolute;
            else if (addr_mode == "relative") table._addr_modes[i] = addr_mode::relative;
            else if (addr_mode == "indirect") table._addr_modes[i] = addr_mode::indirect;
            else if (addr_mode == "indexed_indirect") table._addr_modes[i] = addr_mode::indexed_indirect;
            else if (addr_mode == "indirect_indexed") table._addr_modes[i] = addr_mode::indirect_indexed;
            // clang-format on
        }

        return table;
    }

    constexpr addr_mode operator[](size_t opcode) const
    {
        return _addr_modes[opcode];
    }

    constexpr addr_mode operator[](opcode opcode) const
    {
        return _addr_modes[static_cast<size_t>(opcode)];
    }

private:
    std::array<addr_mode, 256> _addr_modes{};
};

static inline constexpr mnemonic_table mnemonics{mnemonic_table::create()};
static inline constexpr addr_mode_table addr_modes{addr_mode_table::create()};

constexpr auto format_as(opcode code)
{
    return static_cast<std::underlying_type_t<opcode>>(code);
}

} // namespace nese::cpu::instruction
