#include <cassert>

template<>
struct fmt::formatter<nese::cpu>
{
    constexpr auto parse(auto& ctx)
    {
        return ctx.begin();
    }

    auto format(const nese::cpu& cpu, auto& ctx) const
    {
        // Follow Nintendulator log format
        // 0         1         2         3         4         5         6         7         8
        // 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
        // C000  4C F5 C5  JMP $C5F5                       A:00 X:00 Y:00 P:24 SP:FD PPU:  0, 21 CYC:7

        auto out = ctx.out();

        append_program_counter(out, cpu);
        append_space(out);

        append_instruction_bytes(out, cpu);
        append_space(out, 2);

        append_opcode(out, cpu);
        append_space(out);

        append_operand(out, cpu);
        append_space(out);

        append_registers(out, cpu);
        append_space(out);

        append_ppu(out, cpu);
        append_space(out);

        append_cycle(out, cpu);

        return out;
    }

    static void append_program_counter(auto& out, const nese::cpu& cpu)
    {
        out = fmt::format_to(out, "{:04X}", cpu.get_registers().pc);
    }

    static void append_instruction_bytes(auto& out, const nese::cpu& cpu)
    {
        constexpr nese::addr_t max_size = 3;

        const nese::addr_t size = 1 + get_operand_size(nese::cpu::get_opcode_addr_mode(cpu.get_byte_from_memory(cpu.get_registers().pc)));

        assert(size <= max_size);

        for (nese::addr_t i = 0; i < size; ++i)
        {
            out = fmt::format_to(out, " {:02X}", cpu.get_byte_from_memory(cpu.get_registers().pc + i));
        }

        append_space(out, (max_size - size) * 3);
    }

    static void append_opcode(auto& out, const nese::cpu& cpu)
    {
        const nese::byte_t opcode = cpu.get_byte_from_memory(cpu.get_registers().pc);
        const auto opcode_str = nese::cpu::get_opcode_string(opcode);

        for (const char c : opcode_str)
        {
            append_char(out, to_upper(c));
        }
    }

    static void append_operand(auto& out, const nese::cpu& cpu)
    {
        constexpr std::size_t operand_str_capacity = 27;
        std::size_t operand_str_size = 0;

        const nese::cpu_registers& registers = cpu.get_registers();
        const nese::addr_mode addr_mode = nese::cpu::get_opcode_addr_mode(cpu.get_byte_from_memory(registers.pc));
        const nese::byte_t opcode = cpu.get_byte_from_memory(registers.pc);
        const nese::addr_t pc = registers.pc + 1;

        switch (addr_mode)
        {
        case nese::addr_mode::implied:
            break;

        case nese::addr_mode::accumulator:
            append_char(out, 'A');
            operand_str_size += 1;
            break;

        case nese::addr_mode::immediate:
            out = fmt::format_to(out, "#${:02X}", cpu.get_byte_from_memory(pc));
            operand_str_size += 4;
            break;

        case nese::addr_mode::relative:
            out = fmt::format_to(out, "${:04X}", static_cast<nese::word_t>(cpu.get_byte_from_memory(pc) + pc + 1));
            operand_str_size += 5;
            break;

        case nese::addr_mode::zero_page:
        case nese::addr_mode::zero_page_x:
        case nese::addr_mode::zero_page_y:
        {
            nese::addr_t addr = cpu.get_byte_from_memory(pc);

            out = fmt::format_to(out, "${:02X}", addr);
            operand_str_size += 3;

            if (addr_mode == nese::addr_mode::zero_page_x)
            {
                addr += registers.x;
                out = fmt::format_to(out, ",X @ {:02X}", addr);
                operand_str_size += 7;
            }
            else if (addr_mode == nese::addr_mode::zero_page_y)
            {
                addr += registers.y;
                out = fmt::format_to(out, ",Y @ {:02X}", addr);
                operand_str_size += 7;
            }

            out = fmt::format_to(out, " = {:02X}", cpu.get_byte_from_memory(addr));
            operand_str_size += 5;
        }
        break;

        case nese::addr_mode::absolute:
        case nese::addr_mode::absolute_x:
        case nese::addr_mode::absolute_y:
        {
            nese::addr_t addr = cpu.get_word_from_memory(pc);

            out = fmt::format_to(out, "${:04X}", addr);
            operand_str_size += 5;

            if (nese::cpu::get_opcode_string(opcode) != "jmp" && nese::cpu::get_opcode_string(opcode) != "jsr")
            {
                if (addr_mode == nese::addr_mode::absolute_x)
                {
                    addr += registers.x;
                    out = fmt::format_to(out, ",X @ {:04X}", addr);
                    operand_str_size += 9;
                }
                else if (addr_mode == nese::addr_mode::absolute_y)
                {
                    addr += registers.y;
                    out = fmt::format_to(out, ",Y @ {:04X}", addr);
                    operand_str_size += 9;
                }

                out = fmt::format_to(out, " = {:02X}", cpu.get_byte_from_memory(addr));
                operand_str_size += 5;
            }
        }
        break;

        case nese::addr_mode::indirect:
        {
            const nese::addr_t addr = cpu.get_word_from_memory(pc);

            if (nese::cpu::get_opcode_string(opcode) != "jmp" || (addr & 0xff) != 0xff)
            {
                out = fmt::format_to(out, "({:04X}) = {:04X}", addr, cpu.get_word_from_memory(addr));
                operand_str_size += 13;
            }
            else
            {
                // Account for JMP hardware bug
                // http://wiki.nesdev.com/w/index.php/Errata
                out = fmt::format_to(out, "({:04X}) = {:04X}", addr, static_cast<nese::word_t>(cpu.get_byte_from_memory(addr) + (static_cast<nese::word_t>(cpu.get_byte_from_memory(addr & 0xff00)) << 8)));
                operand_str_size += 13;
            }
        }
        break;

        case nese::addr_mode::indexed_indirect:
        {
            const nese::addr_t addr = cpu.get_byte_from_memory(pc);
            const nese::addr_t final_addr = cpu.get_byte_from_memory((addr + registers.x) & 0xff) + (static_cast<nese::word_t>(cpu.get_byte_from_memory((addr + registers.x + 1) & 0xff)) << 8);

            out = fmt::format_to(out, "(${:02X}, X) @ {:02X} = {:04X} = {:02X}", addr, addr + registers.x, final_addr, cpu.get_byte_from_memory(final_addr));
            operand_str_size += 24;
        }
        break;

        case nese::addr_mode::indirect_indexed:
        {
            const nese::addr_t addr = cpu.get_byte_from_memory(pc);
            const nese::addr_t inter_addr = cpu.get_byte_from_memory(addr) + (static_cast<nese::word_t>(cpu.get_byte_from_memory((addr + 1) & 0xff)) << 8);
            const nese::addr_t final_addr = inter_addr + static_cast<nese::word_t>(registers.y);

            out = fmt::format_to(out, "(${:02X}), Y = {:04X} @ {:04X} = {:02X}", addr, inter_addr, final_addr, cpu.get_byte_from_memory(final_addr));
            operand_str_size += 26;
        }
        break;
        }

        if (operand_str_size < operand_str_capacity)
        {
            append_space(out, operand_str_capacity - operand_str_size);
        }
    }

    static void append_registers(auto& out, const nese::cpu& cpu)
    {
        const auto& registers = cpu.get_registers();

        out = fmt::format_to(out, "A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}", registers.a, registers.x, registers.y, registers.p, registers.s);
    }

    static void append_ppu(auto& out, const nese::cpu& cpu)
    {
        (void)cpu;
        out = fmt::format_to(out, "PPU:{}", "  0,  0");
    }

    static void append_cycle(auto& out, const nese::cpu& cpu)
    {
        out = fmt::format_to(out, "CYC:{}", cpu.get_cycle().count());
    }

    static void append_space(auto& out, nese::size_t count = 1)
    {
        for (size_t i = 0; i < count; ++i)
        {
            append_char(out, ' ');
        }
    }

    static void append_char(auto& out, char c)
    {
        *out = c;
        ++out;
    }

    static constexpr char to_upper(char c)
    {
        return static_cast<char>(c - ('a' - 'A'));
    }

    static nese::addr_t get_operand_size(nese::addr_mode mode)
    {
        switch (mode)
        {
        case nese::addr_mode::implied:
        case nese::addr_mode::accumulator:
            return 0;

        case nese::addr_mode::relative:
        case nese::addr_mode::immediate:
        case nese::addr_mode::zero_page:
        case nese::addr_mode::zero_page_x:
        case nese::addr_mode::zero_page_y:
        case nese::addr_mode::indexed_indirect:
        case nese::addr_mode::indirect_indexed:
            return 1;

        case nese::addr_mode::indirect:
        case nese::addr_mode::absolute:
        case nese::addr_mode::absolute_x:
        case nese::addr_mode::absolute_y:
            return 2;
        }

        assert(false);
        __assume(false);
    }
};
