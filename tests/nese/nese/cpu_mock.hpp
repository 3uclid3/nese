#pragma once

#include <nese/cpu.hpp>
#include <nese/ram.hpp>

namespace nese {

class cpu_mock : public cpu
{
public:
    cpu_mock()
        : cpu(ram)
    {
    }

    ram ram;
};

constexpr const cpu& format_as(const cpu_mock& cpu)
{
    return cpu;
}

} // namespace nese
