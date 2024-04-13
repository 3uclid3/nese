#include <nese/utility/nintendulator.hpp>

#include <fmt/format.h>

#include <nese/cpu/addr_mode.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/instruction/operand.hpp>
#include <nese/cpu/state.hpp>
#include <nese/cycle.hpp>
#include <nese/memory/mapper.hpp>
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

    output& operator=(char v)
    {
        buffer[size++] = v;
        return *this;
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

void append_program_counter(auto& out, const cpu::state& cpu_state, const memory::mapper& memory_mapper [[maybe_unused]])
{
    out = fmt::format_to(out, "{:04X}", cpu_state.registers.pc);
}

void append_instruction_bytes(auto& out, const cpu::state& cpu_state, const memory::mapper& memory_mapper)
{
    constexpr byte_t max_count = 3;
    constexpr byte_t opcode_count = 1;

    const cpu::instruction::opcode opcode = static_cast<cpu::instruction::opcode>(memory_mapper.get_byte(cpu_state.registers.pc));
    const string_view mnemonic = cpu::instruction::mnemonics[opcode];
    const byte_t count = mnemonic.empty() ? opcode_count : opcode_count + cpu::instruction::get_operand_size(cpu::instruction::addr_modes[opcode]);

    assert(count <= max_count);

    for (byte_t i = 0; i < count; ++i)
    {
        out = fmt::format_to(out, " {:02X}", memory_mapper.get_byte(cpu_state.registers.pc + i));
    }

    constexpr byte_t space_per_byte = 3;
    append_space(out, (max_count - count) * space_per_byte);
}

void append_opcode(auto& out, const cpu::state& cpu_state [[maybe_unused]], const memory::mapper& memory_mapper [[maybe_unused]])
{
    const cpu::instruction::opcode opcode = static_cast<cpu::instruction::opcode>(memory_mapper.get_byte(cpu_state.registers.pc));
    const string_view mnemonic = cpu::instruction::mnemonics[opcode];

    for (const char c : mnemonic)
    {
        append_char(out, to_upper(c));
    }
}

void append_operand(auto& out, const cpu::state& cpu_state, const memory::mapper& memory_mapper)
{
    constexpr std::size_t operand_length = 27;
    std::size_t current_length = 0;

    const cpu::instruction::opcode opcode = static_cast<cpu::instruction::opcode>(memory_mapper.get_byte(cpu_state.registers.pc));
    const string_view mnemonic = cpu::instruction::mnemonics[opcode];

    if (!mnemonic.empty())
    {
        const cpu::addr_mode addr_mode = cpu::instruction::addr_modes[opcode];
        const addr_t pc = cpu_state.registers.pc + 1;

        switch (addr_mode)
        {
        case cpu::addr_mode::implied:
            break;

        case cpu::addr_mode::accumulator:
            append_char(out, 'A');
            current_length += 1;
            break;

        case cpu::addr_mode::immediate:
            out = fmt::format_to(out, "#${:02X}", memory_mapper.get_byte(pc));
            current_length += 4;
            break;

        case cpu::addr_mode::relative:
            out = fmt::format_to(out, "${:04X}", static_cast<word_t>(memory_mapper.get_byte(pc) + pc + 1));
            current_length += 5;
            break;

        case cpu::addr_mode::zero_page:
        case cpu::addr_mode::zero_page_x:
        case cpu::addr_mode::zero_page_y:
        {
            addr_t addr = memory_mapper.get_byte(pc);

            out = fmt::format_to(out, "${:02X}", addr);
            current_length += 3;

            if (addr_mode == cpu::addr_mode::zero_page_x)
            {
                addr += cpu_state.registers.x;
                out = fmt::format_to(out, ",X @ {:02X}", addr);
                current_length += 7;
            }
            else if (addr_mode == cpu::addr_mode::zero_page_y)
            {
                addr += cpu_state.registers.y;
                out = fmt::format_to(out, ",Y @ {:02X}", addr);
                current_length += 7;
            }

            out = fmt::format_to(out, " = {:02X}", memory_mapper.get_byte(addr));
            current_length += 5;
        }
        break;

        case cpu::addr_mode::absolute:
        case cpu::addr_mode::absolute_x:
        case cpu::addr_mode::absolute_y:
        {
            addr_t addr = memory_mapper.get_word(pc);

            out = fmt::format_to(out, "${:04X}", addr);
            current_length += 5;

            if (mnemonic != "jmp" && mnemonic != "jsr")
            {
                if (addr_mode == cpu::addr_mode::absolute_x)
                {
                    addr += cpu_state.registers.x;
                    out = fmt::format_to(out, ",X @ {:04X}", addr);
                    current_length += 9;
                }
                else if (addr_mode == cpu::addr_mode::absolute_y)
                {
                    addr += cpu_state.registers.y;
                    out = fmt::format_to(out, ",Y @ {:04X}", addr);
                    current_length += 9;
                }

                out = fmt::format_to(out, " = {:02X}", memory_mapper.get_byte(addr));
                current_length += 5;
            }
        }
        break;

        case cpu::addr_mode::indirect:
        {
            const addr_t addr = memory_mapper.get_word(pc);

            if (mnemonic != "jmp" || (addr & 0xff) != 0xff)
            {
                out = fmt::format_to(out, "({:04X}) = {:04X}", addr, memory_mapper.get_word(addr));
                current_length += 13;
            }
            else
            {
                // Account for JMP hardware bug
                // http://wiki.nesdev.com/w/index.php/Errata
                out = fmt::format_to(out, "({:04X}) = {:04X}", addr, static_cast<word_t>(memory_mapper.get_byte(addr) + (static_cast<word_t>(memory_mapper.get_byte(addr & 0xff00)) << 8)));
                current_length += 13;
            }
        }
        break;

        case cpu::addr_mode::indexed_indirect:
        {
            const addr_t addr = memory_mapper.get_byte(pc);
            const addr_t final_addr = static_cast<nese::word_t>(memory_mapper.get_byte((addr + cpu_state.registers.x) & 0xff)) + static_cast<nese::word_t>(static_cast<nese::word_t>(memory_mapper.get_byte((addr + cpu_state.registers.x + 1) & 0xff)) << 8);

            out = fmt::format_to(out, "(${:02X},X) @ {:02X} = {:04X} = {:02X}", addr, (addr + cpu_state.registers.x) & 0xFF, final_addr, memory_mapper.get_byte(final_addr));
            current_length += 24;
        }
        break;

        case cpu::addr_mode::indirect_indexed:
        {
            const addr_t addr = memory_mapper.get_byte(pc);
            const addr_t inter_addr = static_cast<nese::word_t>(memory_mapper.get_byte(addr)) + static_cast<nese::word_t>(static_cast<word_t>(memory_mapper.get_byte((addr + 1) & 0xff)) << 8);
            const addr_t final_addr = inter_addr + static_cast<word_t>(cpu_state.registers.y);

            out = fmt::format_to(out, "(${:02X}), Y = {:04X} @ {:04X} = {:02X}", addr, inter_addr, final_addr, memory_mapper.get_byte(final_addr));
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

void append_registers(auto& out, const cpu::state& cpu_state, const memory::mapper& memory_mapper [[maybe_unused]])
{
    const auto& [a, x, y, pc, s, p] = cpu_state.registers;

    out = fmt::format_to(out, "A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}", a, x, y, p, s);
}

void append_ppu(auto& out, const cpu::state& cpu_state [[maybe_unused]], const memory::mapper& memory_mapper [[maybe_unused]])
{
    out = fmt::format_to(out, "PPU:{}", "  0,  0");
}

void append_cycle(auto& out, const cpu::state& cpu_state, const memory::mapper& memory_mapper [[maybe_unused]])
{
    out = fmt::format_to(out, "CYC:{}", std::chrono::duration_cast<cpu_cycle_t>(cpu_state.cycle).count());
}
} // namespace details

const char* format(const cpu::state& cpu_state, const memory::mapper& memory_mapper)
{
    // Follow Nintendulator log format
    // 0         1         2         3         4         5         6         7         8
    // 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    // C000  4C F5 C5  JMP $C5F5                       A:00 X:00 Y:00 P:24 SP:FD PPU:  0, 21 CYC:7

    details::output out(details::data.next_buffer());

    append_program_counter(out, cpu_state, memory_mapper);
    append_space(out);

    append_instruction_bytes(out, cpu_state, memory_mapper);
    append_space(out, 2);

    append_opcode(out, cpu_state, memory_mapper);
    append_space(out);

    append_operand(out, cpu_state, memory_mapper);
    append_space(out);

    append_registers(out, cpu_state, memory_mapper);
    append_space(out);

    append_ppu(out, cpu_state, memory_mapper);
    append_space(out);

    append_cycle(out, cpu_state, memory_mapper);

    out = '\0';

    return out.buffer.data();
}

} // namespace nese::nintendulator
