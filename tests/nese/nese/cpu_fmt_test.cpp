#include <catch2/catch_test_macros.hpp>

#include <fmt/format.h>

#include <nese/cpu_mock.hpp>
#include <nese/ram.hpp>

namespace nese {

TEST_CASE("cpu - fmt")
{
    // cpu_mock cpu;

    // CHECK(fmt::format("{}", cpu) == "0000  00 00     JMP $C5F5                       A:00 X:00 Y:00 P:00 SP:00 PPU:  0, 21 CYC:0");
}

} // namespace nese