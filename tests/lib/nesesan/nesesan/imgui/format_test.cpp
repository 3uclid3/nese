#include <catch2/catch_test_macros.hpp>

#include <string>

#include <nese/basic_types.hpp>
#include <nesesan/imgui/format.hpp>

namespace nese::san::imgui {

TEST_CASE("imgui::format", "[imgui]")
{
    CHECK(strcmp(format("Lorem {}", "Ipsum"), "Lorem Ipsum") == 0);
    CHECK(strcmp(format("u8 = {}, s8 = {}", u8_t(42), s8_t(-42)), "u8 = 42, s8 = -42") == 0);
    CHECK(strcmp(format("u16 = {}, s16 = {}", u16_t(42), s16_t(-42)), "u16 = 42, s16 = -42") == 0);
    CHECK(strcmp(format("u32 = {}, s32 = {}", u32_t(42), s32_t(-42)), "u32 = 42, s32 = -42") == 0);
    CHECK(strcmp(format("u64 = {}, s64 = {}", u64_t(42), s64_t(-42)), "u64 = 42, s64 = -42") == 0);
}

} // namespace nese::san