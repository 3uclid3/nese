#include <nese/cpu/instruction.hpp>
#include <nese/memory/mapper.hpp>
#include <nese/utility/assert.hpp>
#include <nese/utility/format.hpp>

namespace nese::cpu::details { namespace {

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

void append_program_counter(auto& out, const state& state)
{
    out = fmt::format_to(out, "{:04X}", state.registers.pc);
}

void append_instruction_bytes(auto& out, const state& state)
{
    constexpr byte_t max_count = 3;
    constexpr byte_t opcode_count = 1;

    const byte_t count = opcode_count + instruction::get_opcode_operand_byte_count(state.memory.get().get_byte(state.registers.pc));

    assert(count <= max_count);

    for (byte_t i = 0; i < count; ++i)
    {
        out = fmt::format_to(out, " {:02X}", state.memory.get().get_byte(state.registers.pc + i));
    }

    constexpr byte_t space_per_byte = 3;
    append_space(out, (max_count - count) * space_per_byte);
}

void append_opcode(auto& out, const state& state)
{
    const instruction::opcode_t opcode = state.memory.get().get_byte(state.registers.pc);
    const auto mnemonic = instruction::get_opcode_mnemonic(opcode);

    for (const char c : mnemonic)
    {
        append_char(out, to_upper(c));
    }
}

void append_operand(auto& out, const state& state)
{
    constexpr std::size_t operand_length = 27;
    std::size_t current_length = 0;

    const instruction::opcode_t opcode = state.memory.get().get_byte(state.registers.pc);
    const addr_mode addr_mode = instruction::get_opcode_addr_mode(opcode);
    const addr_t pc = state.registers.pc + 1;

    switch (addr_mode)
    {
    case addr_mode::implied:
        break;

    case addr_mode::accumulator:
        append_char(out, 'A');
        current_length += 1;
        break;

    case addr_mode::immediate:
        out = fmt::format_to(out, "#${:02X}", state.memory.get().get_byte(pc));
        current_length += 4;
        break;

    case addr_mode::relative:
        out = fmt::format_to(out, "${:04X}", static_cast<word_t>(state.memory.get().get_byte(pc) + pc + 1));
        current_length += 5;
        break;

    case addr_mode::zero_page:
    case addr_mode::zero_page_x:
    case addr_mode::zero_page_y:
    {
        addr_t addr = state.memory.get().get_byte(pc);

        out = fmt::format_to(out, "${:02X}", addr);
        current_length += 3;

        if (addr_mode == cpu::addr_mode::zero_page_x)
        {
            addr += state.registers.x;
            out = fmt::format_to(out, ",X @ {:02X}", addr);
            current_length += 7;
        }
        else if (addr_mode == cpu::addr_mode::zero_page_y)
        {
            addr += state.registers.y;
            out = fmt::format_to(out, ",Y @ {:02X}", addr);
            current_length += 7;
        }

        out = fmt::format_to(out, " = {:02X}", state.memory.get().get_byte(addr));
        current_length += 5;
    }
    break;

    case addr_mode::absolute:
    case addr_mode::absolute_x:
    case addr_mode::absolute_y:
    {
        addr_t addr = state.memory.get().get_word(pc);

        out = fmt::format_to(out, "${:04X}", addr);
        current_length += 5;

        const auto mnemonic = instruction::get_opcode_mnemonic(opcode);

        if (mnemonic != "jmp" && mnemonic != "jsr")
        {
            if (addr_mode == addr_mode::absolute_x)
            {
                addr += state.registers.x;
                out = fmt::format_to(out, ",X @ {:04X}", addr);
                current_length += 9;
            }
            else if (addr_mode == addr_mode::absolute_y)
            {
                addr += state.registers.y;
                out = fmt::format_to(out, ",Y @ {:04X}", addr);
                current_length += 9;
            }

            out = fmt::format_to(out, " = {:02X}", state.memory.get().get_byte(addr));
            current_length += 5;
        }
    }
    break;

    case addr_mode::indirect:
    {
        const addr_t addr = state.memory.get().get_word(pc);

        const auto mnemonic = instruction::get_opcode_mnemonic(opcode);

        if (mnemonic != "jmp" || (addr & 0xff) != 0xff)
        {
            out = fmt::format_to(out, "({:04X}) = {:04X}", addr, state.memory.get().get_word(addr));
            current_length += 13;
        }
        else
        {
            // Account for JMP hardware bug
            // http://wiki.nesdev.com/w/index.php/Errata
            out = fmt::format_to(out, "({:04X}) = {:04X}", addr, static_cast<word_t>(state.memory.get().get_byte(addr) + (static_cast<word_t>(state.memory.get().get_byte(addr & 0xff00)) << 8)));
            current_length += 13;
        }
    }
    break;

    case addr_mode::indexed_indirect:
    {
        const addr_t addr = state.memory.get().get_byte(pc);
        const addr_t final_addr = static_cast<nese::word_t>(state.memory.get().get_byte((addr + state.registers.x) & 0xff)) + 
                                  static_cast<nese::word_t>(static_cast<nese::word_t>(state.memory.get().get_byte((addr + state.registers.x + 1) & 0xff)) << 8);

        out = fmt::format_to(out, "(${:02X},X) @ {:02X} = {:04X} = {:02X}", addr, addr + state.registers.x, final_addr, state.memory.get().get_byte(final_addr));
        current_length += 24;
    }
    break;

    case addr_mode::indirect_indexed:
    {
        const addr_t addr = state.memory.get().get_byte(pc);
        const addr_t inter_addr = static_cast<nese::word_t>(state.memory.get().get_byte(addr)) + static_cast<nese::word_t>(static_cast<word_t>(state.memory.get().get_byte((addr + 1) & 0xff)) << 8);
        const addr_t final_addr = inter_addr + static_cast<word_t>(state.registers.y);

        out = fmt::format_to(out, "(${:02X}), Y = {:04X} @ {:04X} = {:02X}", addr, inter_addr, final_addr, state.memory.get().get_byte(final_addr));
        current_length += 26;
    }
    break;
    }

    if (current_length < operand_length)
    {
        append_space(out, operand_length - current_length);
    }
}

void append_registers(auto& out, const state& state)
{
    const auto& [a, x, y, pc, s, p] = state.registers;

    out = fmt::format_to(out, "A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}", a, x, y, p, s);
}

void append_ppu(auto& out, const state& state)
{
    (void)state;
    out = fmt::format_to(out, "PPU:{}", "  0,  0");
}

void append_cycle(auto& out, const state& state)
{
    const cycle_t cycle_count_in_scanline = state.cycle % ppu_scanline_cycle;

    out = fmt::format_to(out, "CYC:{}", cycle_count_in_scanline.count());
}

}} // namespace nese::cpu::details

template<>
struct fmt::formatter<nese::cpu::state>
{
    constexpr auto parse(auto& ctx)
    {
        return ctx.begin();
    }

    // Follow Nintendulator log format
    // 0         1         2         3         4         5         6         7         8
    // 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    // C000  4C F5 C5  JMP $C5F5                       A:00 X:00 Y:00 P:24 SP:FD PPU:  0, 21 CYC:7
    auto format(const nese::cpu::state& state, auto& ctx) const
    {
        using namespace nese::cpu::details;

        auto out = ctx.out();

        append_program_counter(out, state);
        append_space(out);

        append_instruction_bytes(out, state);
        append_space(out, 2);

        append_opcode(out, state);
        append_space(out);

        append_operand(out, state);
        append_space(out);

        append_registers(out, state);
        append_space(out);

        append_ppu(out, state);
        append_space(out);

        append_cycle(out, state);

        return out;
    }
};
