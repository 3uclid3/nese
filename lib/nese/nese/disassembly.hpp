#pragma once

#include <variant>

#include <nese/basic_types.hpp>
#include <nese/cpu/cpu_opcode.hpp>

namespace nese {

struct bus;

class disassembly
{
public:
    struct line
    {
        [[nodiscard]] const char* to_string() const;

        addr_t addr{};
        std::variant<byte_t, word_t> operand{};
        cpu_opcode opcode{};
    };

    using const_iterator = std::vector<line>::const_iterator;

public:
    static disassembly disassemble(const bus& bus, addr_t start_addr = 0x8000, addr_t end_addr = 0xFFFF);

public:
    [[nodiscard]] bool is_empty() const;

    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;

private:
    std::vector<line> _lines;
    addr_t _start_addr{0x8000};
    addr_t _end_addr{0xFFFF};
};

inline bool disassembly::is_empty() const
{
    return _lines.empty();
}

inline disassembly::const_iterator disassembly::begin() const
{
    return _lines.begin();    
}

inline disassembly::const_iterator disassembly::end() const
{
    return _lines.end();
}

} // namespace nese