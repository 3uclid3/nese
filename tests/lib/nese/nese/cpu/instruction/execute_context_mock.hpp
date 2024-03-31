#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/instruction/execute_context.hpp>
#include <nese/memory/mapper.hpp>

namespace nese::cpu::instruction {

class execute_context_mock : public execute_context
{
public:
    constexpr execute_context_mock()
        : execute_context(_owned_state, _owned_memory)
    {
    }

    constexpr execute_context_mock(execute_context_mock&& other)
        : execute_context_mock()
    {
        _owned_state = other._owned_state;
        _owned_memory = other._owned_memory;
    }

    constexpr execute_context_mock(const execute_context_mock& other)
        : execute_context_mock()
    {
        _owned_state = other._owned_state;
        _owned_memory = other._owned_memory;
    }

    constexpr execute_context_mock& operator=(execute_context_mock&& other)
    {
        _owned_state = other._owned_state;
        _owned_memory = other._owned_memory;
        return *this;
    }

    constexpr execute_context_mock& operator=(const execute_context_mock& other)
    {
        _owned_state = other._owned_state;
        _owned_memory = other._owned_memory;
        return *this;
    }

    constexpr ~execute_context_mock() = default;

    [[nodiscard]] constexpr const cpu::state& state() const { return _owned_state; }
    [[nodiscard]] constexpr cpu::state& state() { return _owned_state; }

private:
    cpu::state _owned_state;
    memory::mapper _owned_memory;
};

} // namespace nese::cpu::instruction
