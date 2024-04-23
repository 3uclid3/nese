#pragma once

#include <nese/basic_types.hpp>
#include <nese/utility/format.hpp>

namespace nese {

class color
{
public:
    static constexpr u8_t default_alpha = 0xFF;
    static constexpr u8_t default_r = 0x00;
    static constexpr u8_t default_g = 0x00;
    static constexpr u8_t default_b = 0x00;

    constexpr color() = default;
    constexpr color(const color&) = default;
    constexpr color(color&&) = default;
    constexpr color& operator=(const color&) = default;
    constexpr color& operator=(color&&) = default;

    constexpr explicit color(u32_t color);
    constexpr color(u8_t r, u8_t g, u8_t b, u8_t a = default_alpha);

    constexpr bool operator==(const color& other) const;
    constexpr bool operator!=(const color& other) const;

    constexpr explicit operator u32_t() const;

    [[nodiscard]] constexpr u8_t alpha() const;
    [[nodiscard]] constexpr u8_t a() const;
    [[nodiscard]] constexpr u8_t r() const;
    [[nodiscard]] constexpr u8_t g() const;
    [[nodiscard]] constexpr u8_t b() const;

    constexpr void set_alpha(u8_t value);
    constexpr void set_a(u8_t value);
    constexpr void set_r(u8_t value);
    constexpr void set_g(u8_t value);
    constexpr void set_b(u8_t value);

    constexpr color operator+(const color& other) const;
    constexpr color& operator+=(const color& other);

    constexpr color operator-(const color& other) const;
    constexpr color& operator-=(const color& other);

    constexpr color operator*(f32_t factor) const;
    constexpr color& operator*=(f32_t factor);

    constexpr color operator/(f32_t factor) const;
    constexpr color& operator/=(f32_t factor);

    constexpr color inv() const;

private:
    union
    {
        struct
        {
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            u8_t _a, _r, _g, _b;
#elif (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            u8_t _b, _g, _r, _a;
#endif
        };
        u32_t _value{static_cast<u32_t>(default_r | (default_g << 8) | (default_b << 16) | (default_alpha << 24))};
    };
};

static_assert(sizeof(color) == sizeof(u32_t));

constexpr color::color(u32_t color)
    : _value(color)
{
}

constexpr color::color(u8_t r, u8_t g, u8_t b, u8_t a)
// clang-format off
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    : _a(a), _r(r), _g(g), _b(b)
#elif (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    : _b(b), _g(g), _r(r), _a(a)
#endif
// clang-format on
{}

constexpr bool color::operator==(const color& other) const
{
    return _value == other._value;
}

constexpr bool color::operator!=(const color& other) const
{
    return _value != other._value;
}

constexpr color::operator u32_t() const
{
    return _value;
}

constexpr u8_t color::alpha() const
{
    return _a;
}

constexpr u8_t color::a() const
{
    return _a;
}

constexpr u8_t color::r() const
{
    return _r;
}

constexpr u8_t color::g() const
{
    return _g;
}

constexpr u8_t color::b() const
{
    return _b;
}

constexpr void color::set_alpha(u8_t value)
{
    set_a(value);
}

constexpr void color::set_a(u8_t value)
{
    _a = value;
}

constexpr void color::set_r(u8_t value)
{
    _r = value;
}

constexpr void color::set_g(u8_t value)
{
    _g = value;
}

constexpr void color::set_b(u8_t value)
{
    _b = value;
}

constexpr color color::operator+(const color& other) const
{
    color color = *this;
    color += other;

    return color;
}

constexpr color& color::operator+=(const color& other)
{
    _a = static_cast<u8_t>(std::min(255, static_cast<int>(_a) + other._a));
    _r = static_cast<u8_t>(std::min(255, static_cast<int>(_r) + other._r));
    _g = static_cast<u8_t>(std::min(255, static_cast<int>(_g) + other._g));
    _b = static_cast<u8_t>(std::min(255, static_cast<int>(_b) + other._b));

    return *this;
}

constexpr color color::operator-(const color& other) const
{
    color color = *this;
    color -= other;

    return color;
}

constexpr color& color::operator-=(const color& other)
{
    _a = static_cast<u8_t>(std::max(0, static_cast<int>(_a) - other._a));
    _r = static_cast<u8_t>(std::max(0, static_cast<int>(_r) - other._r));
    _g = static_cast<u8_t>(std::max(0, static_cast<int>(_g) - other._g));
    _b = static_cast<u8_t>(std::max(0, static_cast<int>(_b) - other._b));

    return *this;
}

constexpr color color::operator*(f32_t factor) const
{
    color color = *this;
    color *= factor;

    return color;
}

constexpr color& color::operator*=(f32_t factor)
{
    _a = static_cast<u8_t>(clamp(static_cast<f32_t>(_a) * factor, 0.f, 250.f));
    _r = static_cast<u8_t>(clamp(static_cast<f32_t>(_r) * factor, 0.f, 250.f));
    _g = static_cast<u8_t>(clamp(static_cast<f32_t>(_g) * factor, 0.f, 250.f));
    _b = static_cast<u8_t>(clamp(static_cast<f32_t>(_b) * factor, 0.f, 250.f));

    return *this;
}

constexpr color color::operator/(f32_t factor) const
{
    color color = *this;
    color /= factor;

    return color;
}

constexpr color& color::operator/=(f32_t factor)
{
    _a = static_cast<u8_t>(clamp(static_cast<f32_t>(_a) / factor, 0.f, 250.f));
    _r = static_cast<u8_t>(clamp(static_cast<f32_t>(_r) / factor, 0.f, 250.f));
    _g = static_cast<u8_t>(clamp(static_cast<f32_t>(_g) / factor, 0.f, 250.f));
    _b = static_cast<u8_t>(clamp(static_cast<f32_t>(_b) / factor, 0.f, 250.f));

    return *this;
}

constexpr auto format_as(const color& color)
{
    return format("{}, {}, {}, {}", color.a(), color.r(), color.g(), color.b());
}

constexpr color clear_alpha(color color)
{
    color.set_alpha(0);
    return color;
}

struct color_palette
{
    static constexpr color blank{0, 0, 0, 0};

    static constexpr color white{255, 255, 255};
    static constexpr color black{0, 0, 0};

    static constexpr color blue{0, 0, 255};
    static constexpr color dark_blue{0, 0, 128};
    static constexpr color very_dark_blue{0, 0, 64};

    static constexpr color cyan{0, 255, 255};
    static constexpr color dark_cyan{0, 128, 128};
    static constexpr color very_dark_cyan{0, 64, 64};

    static constexpr color green{0, 255, 0};
    static constexpr color dark_green{0, 128, 0};
    static constexpr color very_dark_green{0, 64, 0};

    static constexpr color grey{192, 192, 192};
    static constexpr color dark_grey{128, 128, 128};
    static constexpr color very_dark_grey{64, 64, 64};

    static constexpr color magenta{255, 0, 255};
    static constexpr color dark_magenta{128, 0, 128};
    static constexpr color very_dark_magenta{64, 0, 64};

    static constexpr color red{255, 0, 0};
    static constexpr color dark_red{128, 0, 0};
    static constexpr color very_dark_red{64, 0, 0};

    static constexpr color yellow{255, 255, 0};
    static constexpr color dark_yellow{128, 128, 0};
    static constexpr color very_dark_yellow{64, 64, 0};
};

} // namespace nese