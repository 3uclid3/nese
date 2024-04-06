#pragma once

#include <iostream>

#include <nese/basic_types.hpp>
#include <nese/utility/format.hpp>

namespace nese {

template<std::integral T>
class hex
{
public:
    constexpr hex() = default;

    constexpr hex(T value)
        : _value(value)
    {
    }

    [[nodiscard]] constexpr T value() const
    {
        return _value;
    }

    [[nodiscard]] constexpr operator T() const
    {
        return _value;
    }

private:
    T _value{};
};

using byte_x = hex<byte_t>;
using word_x = hex<word_t>;
using addr_x = hex<addr_t>;

template<std::integral T>
std::ostream& operator<<(std::ostream& os, hex<T> hex)
{
    if constexpr (sizeof(T) == 1)
    {
        os << format("{:02X}", hex.value());
    }
    else if constexpr (sizeof(T) == 2)
    {
        os << format("{:04X}", hex.value());
    }
    else if constexpr (sizeof(T) == 4)
    {
        os << format("{:08X}", hex.value());
    }
    else if constexpr (sizeof(T) == 8)
    {
        os << format("{:016X}", hex.value());
    }
    else
    {
        os << format("{:0X}", hex.value());
    }

    return os;
}

} // namespace nese
