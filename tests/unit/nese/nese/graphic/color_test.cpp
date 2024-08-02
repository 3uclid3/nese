#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/graphic/color.hpp>

namespace nese {

inline std::ostream& operator<<(std::ostream& os, const color& color)
{
    os << format("{}", color);
    return os;
}

TEST_CASE("color addition", "[graphic]")
{
    color yellow = color_palette::red + color_palette::green;
    color magenta = color_palette::red + color_palette::blue;

    CHECK(yellow == color_palette::yellow);
    CHECK(magenta == color_palette::magenta);

    SECTION("saturation")
    {
        color red = color_palette::red + color_palette::red;
        color green = color_palette::green + color_palette::green;
        color blue = color_palette::blue + color_palette::blue;

        CHECK(red == color_palette::red);
        CHECK(green == color_palette::green);
        CHECK(blue == color_palette::blue);
    }
}

TEST_CASE("color substraction", "[graphic]")
{
    {
        color green = color_palette::yellow - color_palette::red;
        color blue = color_palette::magenta - color_palette::red;

        CHECK(green == clear_alpha(color_palette::green));
        CHECK(blue == clear_alpha(color_palette::blue));
    }

    SECTION("saturation")
    {
        color red = color_palette::red - color_palette::green;
        color green = color_palette::green - color_palette::red;
        color blue = color_palette::blue - color_palette::yellow;
        color yellow = color_palette::yellow - color_palette::blue;

        CHECK(red == clear_alpha(color_palette::red));
        CHECK(green == clear_alpha(color_palette::green));
        CHECK(blue == clear_alpha(color_palette::blue));
        CHECK(yellow == clear_alpha(color_palette::yellow));
    }
}

} // namespace nese