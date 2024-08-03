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
concept hexadecimalable = std::is_arithmetic_v<T> || std::is_enum_v<T>;

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

} // namespace nese

template<typename T>
struct fmt::formatter<nese::hex<T>>
{
    constexpr auto parse(auto& ctx)
    {
        return ctx.begin();
    }

    auto format(nese::hex<T> v, auto& ctx) const
    {
        if constexpr (sizeof(T) == 1)
        {
            return fmt::format_to(ctx.out(), "{:02X}", v.value());
        }
        else if constexpr (sizeof(T) == 2)
        {
            return fmt::format_to(ctx.out(), "{:04X}", v.value());
        }
        else if constexpr (sizeof(T) == 4)
        {
            return fmt::format_to(ctx.out(), "{:08X}", v.value());
        }
        else if constexpr (sizeof(T) == 8)
        {
            return fmt::format_to(ctx.out(), "{:016X}", v.value());
        }
        else
        {
            return fmt::format_to(ctx.out(), "{:0X}", v.value());
        }
    }
};

namespace nese {

template<hexadecimalable T>
std::ostream& operator<<(std::ostream& os, hex<T> v)
{
    os << format("{}", v);
    return os;
}

} // namespace nese
