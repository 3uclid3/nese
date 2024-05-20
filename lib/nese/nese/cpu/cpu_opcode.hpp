#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/cpu_addr_mode.hpp>
#include <nese/cpu/cpu_opcode_enum.hpp>

namespace nese {

template<typename T>
class cpu_opcode_table
{
public:
    constexpr T operator[](size_t opcode) const
    {
        return _data[opcode];
    }

    constexpr T& operator[](size_t opcode)
    {
        return _data[opcode];
    }

    constexpr T operator[](cpu_opcode opcode) const
    {
        return _data[static_cast<size_t>(opcode)];
    }

    constexpr T& operator[](cpu_opcode opcode)
    {
        return _data[static_cast<size_t>(opcode)];
    }

protected:
    std::array<T, 256> _data{};
};

class cpu_opcode_mnemonic_table : public cpu_opcode_table<string_view>
{
public:
    static consteval cpu_opcode_mnemonic_table create()
    {
        cpu_opcode_mnemonic_table table;

        for (size_t i = 0; i < table._data.size(); ++i)
        {
            const string_view name = magic_enum::enum_name(static_cast<cpu_opcode>(i));

            if (name.empty())
                continue;

            const size_t index = name.find_first_of('_');

            table._data[i] = name.substr(0, index);
        }

        return table;
    }
};

class cpu_opcode_addr_mode_table : public cpu_opcode_table<cpu_addr_mode>
{
public:
    static consteval cpu_opcode_addr_mode_table create()
    {
        cpu_opcode_addr_mode_table table;

        for (size_t i = 0; i < table._data.size(); ++i)
        {
            const string_view name = magic_enum::enum_name(static_cast<cpu_opcode>(i));

            if (name.empty())
                continue;

            const size_t index = name.find_first_of('_');
            const string_view addr_mode = name.substr(index + 1); // skip the '_'

            // clang-format off
            if (addr_mode.starts_with("implied")) table._data[i] = cpu_addr_mode::implied;
            else if (addr_mode.starts_with("accumulator")) table._data[i] = cpu_addr_mode::accumulator;
            else if (addr_mode.starts_with("immediate")) table._data[i] = cpu_addr_mode::immediate;
            else if (addr_mode.starts_with("zero_page_x")) table._data[i] = cpu_addr_mode::zero_page_x;
            else if (addr_mode.starts_with("zero_page_y")) table._data[i] = cpu_addr_mode::zero_page_y;
            else if (addr_mode.starts_with("zero_page")) table._data[i] = cpu_addr_mode::zero_page;
            else if (addr_mode.starts_with("absolute_x")) table._data[i] = cpu_addr_mode::absolute_x;
            else if (addr_mode.starts_with("absolute_y")) table._data[i] = cpu_addr_mode::absolute_y;
            else if (addr_mode.starts_with("absolute")) table._data[i] = cpu_addr_mode::absolute;
            else if (addr_mode.starts_with("relative")) table._data[i] = cpu_addr_mode::relative;
            else if (addr_mode.starts_with("indexed_indirect")) table._data[i] = cpu_addr_mode::indexed_indirect;
            else if (addr_mode.starts_with("indirect_indexed")) table._data[i] = cpu_addr_mode::indirect_indexed;
            else if (addr_mode.starts_with("indirect")) table._data[i] = cpu_addr_mode::indirect;
            // clang-format on
        }

        return table;
    }
};

class cpu_opcode_is_official_table : public cpu_opcode_table<bool>
{
public:
    static consteval cpu_opcode_is_official_table create()
    {
        cpu_opcode_is_official_table table;

        for (size_t i = 0; i < table._data.size(); ++i)
        {
            const string_view name = magic_enum::enum_name(static_cast<cpu_opcode>(i));

            if (name.empty())
            {
                continue;
            }

            table._data[i] = !name.contains("unofficial");
        }

        return table;
    }
};

static inline constexpr cpu_opcode_mnemonic_table cpu_opcode_mnemonics{cpu_opcode_mnemonic_table::create()};
static inline constexpr cpu_opcode_addr_mode_table cpu_opcode_addr_modes{cpu_opcode_addr_mode_table::create()};
static inline constexpr cpu_opcode_is_official_table cpu_opcode_is_officials{cpu_opcode_is_official_table::create()};

} // namespace nese
