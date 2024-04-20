#include <nese/utility/nintendulator.hpp>

#include <span>

#include <fmt/format.h>

#include <nese/bus.hpp>
#include <nese/cycle.hpp>
#include <nese/utility/assert.hpp>

namespace nese::nintendulator {
namespace details {

struct local_data
{
    static constexpr size_t size = 100;
    static constexpr size_t count = 16;

    static constexpr size_t last_buffer_index = count - 1;

    std::span<char> next_buffer()
    {
        const size_t offset = next_buffer_index * size;

        next_buffer_index = next_buffer_index == last_buffer_index ? 0 : next_buffer_index + 1;

        return {&buffer[offset], size};
    }

    std::array<char, size * count> buffer{};
    size_t next_buffer_index{0};
};

struct output
{
    explicit output(std::span<char> buffer, size_t size = 0)
        : buffer(buffer)
        , size(size)
    {
    }

    [[nodiscard]] char& previous()
    {
        return buffer[size - 1];
    }

    [[nodiscard]] char& operator*()
    {
        return buffer[size];
    }

    output& operator++()
    {
        ++size;
        return *this;
    }

    output operator++(int)
    {
        output out(buffer, size);

        ++size;

        return out;
    }

    std::span<char> buffer{};
    size_t size{0};
};

thread_local local_data data{};

constexpr char to_upper(char c)
{
    return static_cast<char>(c - ('a' - 'A'));
}

constexpr byte_t get_operand_size(cpu_addr_mode mode)
{
    switch (mode)
    {
    case cpu_addr_mode::implied:
    case cpu_addr_mode::accumulator:
        return 0;

    case cpu_addr_mode::relative:
    case cpu_addr_mode::immediate:
    case cpu_addr_mode::zero_page:
    case cpu_addr_mode::zero_page_x:
    case cpu_addr_mode::zero_page_y:
    case cpu_addr_mode::indexed_indirect:
    case cpu_addr_mode::indirect_indexed:
        return 1;

    case cpu_addr_mode::indirect:
    case cpu_addr_mode::absolute:
    case cpu_addr_mode::absolute_x:
    case cpu_addr_mode::absolute_y:
        return 2;
    }

    NESE_ASSUME(false);
}

constexpr void append_char(auto& out, char c)
{
    *out = c;
    ++out;
}

constexpr void append_space(auto& out, nese::size_t count = 1)
{
    for (size_t i = 0; i < count; ++i)
    {
        append_char(out, ' ');
    }
}

void append_program_counter(auto& out, const bus& bus)
{
    out = fmt::format_to(out, "{:04X}", bus.cpu.get_state().registers.pc);
}

void append_instruction_bytes(auto& out, const bus& bus)
{
    constexpr byte_t max_count = 3;
    constexpr byte_t opcode_count = 1;

    const cpu_opcode opcode = static_cast<cpu_opcode>(bus.read(bus.cpu.get_state().registers.pc));
    const string_view mnemonic = cpu_opcode_mnemonics[opcode];
    const byte_t count = mnemonic.empty() ? opcode_count : opcode_count + get_operand_size(cpu_opcode_addr_modes[opcode]);

    assert(count <= max_count);

    for (byte_t i = 0; i < count; ++i)
    {
        out = fmt::format_to(out, " {:02X}", bus.read(bus.cpu.get_state().registers.pc + i));
    }

    constexpr byte_t space_per_byte = 3;
    append_space(out, (max_count - count) * space_per_byte);
}

void append_opcode(auto& out, const bus& bus)
{
    const cpu_opcode opcode = static_cast<cpu_opcode>(bus.read(bus.cpu.get_state().registers.pc));
    const string_view mnemonic = cpu_opcode_mnemonics[opcode];

    append_char(out, cpu_opcode_is_officials[opcode] ? ' ' : '*');

    for (const char c : mnemonic)
    {
        append_char(out, to_upper(c));
    }
}

void append_operand(auto& out, const bus& bus)
{
    constexpr std::size_t operand_length = 27;
    std::size_t current_length = 0;

    const cpu_opcode opcode = static_cast<cpu_opcode>(bus.read(bus.cpu.get_state().registers.pc));
    const string_view mnemonic = cpu_opcode_mnemonics[opcode];

    if (!mnemonic.empty())
    {
        const cpu_addr_mode addr_mode = cpu_opcode_addr_modes[opcode];
        const addr_t pc = bus.cpu.get_state().registers.pc + 1;

        switch (addr_mode)
        {
        case cpu_addr_mode::implied:
            break;

        case cpu_addr_mode::accumulator:
            append_char(out, 'A');
            current_length += 1;
            break;

        case cpu_addr_mode::immediate:
            out = fmt::format_to(out, "#${:02X}", bus.read(pc));
            current_length += 4;
            break;

        case cpu_addr_mode::relative:
        {
            const s8_t byte = static_cast<s8_t>(bus.read(pc));
            out = fmt::format_to(out, "${:04X}", static_cast<word_t>(pc + byte + 1));
            current_length += 5;
        }
        break;

        case cpu_addr_mode::zero_page:
        case cpu_addr_mode::zero_page_x:
        case cpu_addr_mode::zero_page_y:
        {
            addr_t addr = bus.read(pc);

            out = fmt::format_to(out, "${:02X}", addr);
            current_length += 3;

            if (addr_mode == cpu_addr_mode::zero_page_x)
            {
                addr = addr + bus.cpu.get_state().registers.x & 0xFF;
                out = fmt::format_to(out, ",X @ {:02X}", addr);
                current_length += 7;
            }
            else if (addr_mode == cpu_addr_mode::zero_page_y)
            {
                addr = addr + bus.cpu.get_state().registers.y & 0xFF;
                out = fmt::format_to(out, ",Y @ {:02X}", addr);
                current_length += 7;
            }

            out = fmt::format_to(out, " = {:02X}", bus.read(addr));
            current_length += 5;
        }
        break;

        case cpu_addr_mode::absolute:
        case cpu_addr_mode::absolute_x:
        case cpu_addr_mode::absolute_y:
        {
            addr_t addr = bus.read_word(pc);

            out = fmt::format_to(out, "${:04X}", addr);
            current_length += 5;

            if (mnemonic != "jmp" && mnemonic != "jsr")
            {
                if (addr_mode == cpu_addr_mode::absolute_x)
                {
                    addr += bus.cpu.get_state().registers.x;
                    out = fmt::format_to(out, ",X @ {:04X}", addr);
                    current_length += 9;
                }
                else if (addr_mode == cpu_addr_mode::absolute_y)
                {
                    addr += bus.cpu.get_state().registers.y;
                    out = fmt::format_to(out, ",Y @ {:04X}", addr);
                    current_length += 9;
                }

                out = fmt::format_to(out, " = {:02X}", bus.read(addr));
                current_length += 5;
            }
        }
        break;

        case cpu_addr_mode::indirect:
        {
            const addr_t addr = bus.read_word(pc);

            if (mnemonic != "jmp" || (addr & 0xff) != 0xff)
            {
                out = fmt::format_to(out, "(${:04X}) = {:04X}", addr, bus.read_word(addr));
                current_length += 14;
            }
            else
            {
                // Account for JMP hardware bug
                // http://wiki.nesdev.com/w/index.php/Errata
                out = fmt::format_to(out, "(${:04X}) = {:04X}", addr, static_cast<word_t>(bus.read(addr) + (static_cast<word_t>(bus.read(addr & 0xff00)) << 8)));
                current_length += 14;
            }
        }
        break;

        case cpu_addr_mode::indexed_indirect:
        {
            const addr_t addr = bus.read(pc);
            const addr_t final_addr = static_cast<nese::word_t>(bus.read((addr + bus.cpu.get_state().registers.x) & 0xff)) + static_cast<nese::word_t>(static_cast<nese::word_t>(bus.read((addr + bus.cpu.get_state().registers.x + 1) & 0xff)) << 8);

            out = fmt::format_to(out, "(${:02X},X) @ {:02X} = {:04X} = {:02X}", addr, (addr + bus.cpu.get_state().registers.x) & 0xFF, final_addr, bus.read(final_addr));
            current_length += 24;
        }
        break;

        case cpu_addr_mode::indirect_indexed:
        {
            const addr_t addr = bus.read(pc);
            const addr_t inter_addr = static_cast<nese::word_t>(bus.read(addr)) + static_cast<nese::word_t>(static_cast<word_t>(bus.read((addr + 1) & 0xff)) << 8);
            const addr_t final_addr = inter_addr + static_cast<word_t>(bus.cpu.get_state().registers.y);

            out = fmt::format_to(out, "(${:02X}),Y = {:04X} @ {:04X} = {:02X}", addr, inter_addr, final_addr, bus.read(final_addr));
            current_length += 26;
        }
        break;
        }
    }

    if (current_length < operand_length)
    {
        append_space(out, operand_length - current_length);
    }
}

void append_registers(auto& out, const bus& bus)
{
    const auto& [a, x, y, pc, s, p] = bus.cpu.get_state().registers;

    out = fmt::format_to(out, "A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}", a, x, y, p, s);
}

void append_ppu(auto& out, const bus& bus[[maybe_unused]])
{
    out = fmt::format_to(out, "PPU:{}", "  0,  0");
}

void append_cycle(auto& out, const bus& bus)
{
    out = fmt::format_to(out, "CYC:{}", bus.cpu.get_state().cycle.count());
}

} // namespace details

const char* format(const bus& bus, format_flags flags)
{
    // Follow Nintendulator log format
    // 0         1         2         3         4         5         6         7         8
    // 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    // C000  4C F5 C5  JMP $C5F5                       A:00 X:00 Y:00 P:24 SP:FD PPU:  0, 21 CYC:7

    details::output out(details::data.next_buffer());

    if ((flags & format_flag::pc) != format_flag::none)
    {
        append_program_counter(out, bus);
        append_space(out);
    }

    if ((flags & format_flag::instruction_bytes) != format_flag::none)
    {
        append_instruction_bytes(out, bus);
        append_space(out);
    }

    if ((flags & format_flag::opcode) != format_flag::none)
    {
        append_opcode(out, bus);
        append_space(out);
    }

    if ((flags & format_flag::operand) != format_flag::none)
    {
        append_operand(out, bus);
        append_space(out);
    }

    if ((flags & format_flag::registers) != format_flag::none)
    {
        append_registers(out, bus);
        append_space(out);
    }

    if ((flags & format_flag::ppu) != format_flag::none)
    {
        append_ppu(out, bus);
        append_space(out);
    }

    if ((flags & format_flag::cycle) != format_flag::none)
    {
        append_cycle(out, bus);
    }

    if (out.previous() != ' ')
    {
        ++out;
    }

    out.previous() = '\0';

    return out.buffer.data();
}

} // namespace nese::nintendulator
