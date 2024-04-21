#include <nese/disassembly.hpp>

#include <nese/bus.hpp>
#include <nese/utility/format.hpp>

namespace nese {

const char* disassembly::line::to_string() const
{
    auto format_operand = [this] -> const char* {
        switch (cpu_opcode_addr_modes[opcode])
        {
        case cpu_addr_mode::accumulator:
        case cpu_addr_mode::implied:
            return "";

        case cpu_addr_mode::immediate:
            return format("#${:02X}", std::get<byte_t>(operand));

        case cpu_addr_mode::zero_page:
            return format("${:02X}", std::get<byte_t>(operand));

        case cpu_addr_mode::zero_page_x:
            return format("${:02X}, X", std::get<byte_t>(operand));

        case cpu_addr_mode::zero_page_y:
            return format("${:02X}, Y", std::get<byte_t>(operand));

        case cpu_addr_mode::relative:
            return format("${:02X} [${:04X}]", std::get<byte_t>(operand), addr - static_cast<s8_t>(std::get<byte_t>(operand)));

        case cpu_addr_mode::indexed_indirect:
            return format("(${:02X}), X)", std::get<byte_t>(operand));

        case cpu_addr_mode::indirect_indexed:
            return format("(${:02X}), Y", std::get<byte_t>(operand));

        case cpu_addr_mode::absolute:
            return format("${:04X}", std::get<word_t>(operand));

        case cpu_addr_mode::absolute_x:
            return format("${:04X}, X", std::get<word_t>(operand));

        case cpu_addr_mode::absolute_y:
            return format("${:04X}, Y", std::get<word_t>(operand));

        case cpu_addr_mode::indirect:
            return format("(${:04X})", std::get<word_t>(operand));
        }
    };

    return nese::format("${:04X}: {:02X} {} {}", addr, opcode, cpu_opcode_mnemonics[opcode], format_operand());
}

disassembly disassembly::disassemble(const bus& bus, addr_t start_addr, addr_t end_addr)
{
    NESE_ASSERT(end_addr >= start_addr);

    const size_t end_addr_s = end_addr;

    disassembly disassembly;

    disassembly._start_addr = start_addr;
    disassembly._end_addr = end_addr;

    for (size_t i = start_addr; i <= end_addr_s;)
    {
        line line;
        line.addr = static_cast<addr_t>(i);
        line.opcode = static_cast<cpu_opcode>(bus.read(static_cast<addr_t>(i)));
        i += 1;

        switch (cpu_opcode_addr_modes[line.opcode])
        {
        case cpu_addr_mode::accumulator:
        case cpu_addr_mode::implied:
            // nothing
            break;

        case cpu_addr_mode::immediate:
        case cpu_addr_mode::zero_page:
        case cpu_addr_mode::zero_page_x:
        case cpu_addr_mode::zero_page_y:
        case cpu_addr_mode::indexed_indirect:
        case cpu_addr_mode::indirect_indexed:
        case cpu_addr_mode::relative:
            line.operand = bus.read(static_cast<addr_t>(i));
            i += 1;
            break;

        case cpu_addr_mode::absolute:
        case cpu_addr_mode::absolute_x:
        case cpu_addr_mode::absolute_y:
        case cpu_addr_mode::indirect:
            line.operand = bus.read_word(static_cast<addr_t>(i));
            i += 2;
            break;
        }

        disassembly._lines.emplace_back(line);
    }

    return disassembly;
}

} // namespace nese