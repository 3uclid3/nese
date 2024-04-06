#pragma once

#include <iostream>

#include <nese/basic_types.hpp>
#include <nese/utility/format.hpp>

namespace nese {

template<typename T>
concept arithmetic_hexadecimalable = std::is_arithmetic_v<T>;

template<typename T>
concept enum_hexadecimalable = std::is_enum_v<T>;

template<typename T>
concept hexadecimalable = std::is_arithmetic_v<T>  || std::is_enum_v<T>;

template<arithmetic_hexadecimalable T>
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

template<enum_hexadecimalable T>
class enum_hex
{
public:
    constexpr enum_hex() = default;

    constexpr enum_hex(T value)
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

    [[nodiscard]] constexpr operator std::underlying_type_t<T>() const
    {
        return static_cast<std::underlying_type_t<T>>(_value);
    }

private:
    T _value{};
};

using byte_x = hex<byte_t>;
using word_x = hex<word_t>;
using addr_x = hex<addr_t>;

template<hexadecimalable T>
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
