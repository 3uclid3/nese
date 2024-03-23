#pragma once

#include <nese/cpu/processor.hpp>
#include <nese/memory/ram.hpp>

namespace nese::cpu {

class processor_mock : public processor
{
public:
    processor_mock()
        : processor(ram)
    {
    }

    memory::ram ram;
};

constexpr const processor& format_as(const processor_mock& cpu)
{
    return cpu;
}

} // namespace nese
