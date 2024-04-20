#include <catch2/catch_test_macros.hpp>

#include <spdlog/sinks/basic_file_sink.h>

#include <nese/bus.hpp>
#include <nese/test_config.hpp>
#include <nese/utility/format.hpp>
#include <nese/utility/hex.hpp>
#include <nese/utility/log.hpp>
#include <nese/utility/nintendulator.hpp>

namespace nese {

TEST_CASE("nestest", "[cpu][instruction][!mayfail]")
{
    constexpr addr_x end_pc = 0x0005;

    bus bus;

    std::shared_ptr<spdlog::logger> nintendulator_logger = spdlog::basic_logger_st("nintendulator_v2", "cpu_nintendulator_nestest_v2.log", true);
    nintendulator_logger->set_level(spdlog::level::trace);
    nintendulator_logger->set_pattern("%v");

    const char* filepath = format("{}/nestest.nes", test_roms_path);
    bus.cartridge = cartridge::from_file(filepath);
    bus.cpu.get_state().registers.pc = bus.cartridge.get_prg().size() == 0x4000 ? 0xc000 : 0x8000;

    auto format_flags = nintendulator::format_flag::all;
    format_flags = format_flags & ~nintendulator::format_flag::ppu;
    format_flags = format_flags & ~nintendulator::format_flag::cycle;

    bool last_instruction_succeeded = true;
    while (last_instruction_succeeded && bus.cpu.get_state().registers.pc != end_pc)
    {
        nintendulator_logger->trace(nintendulator::format(bus, format_flags));

        last_instruction_succeeded = bus.cpu.step();
    }

    REQUIRE(bus.cpu.get_state().registers.pc == end_pc);

    CHECK(bus.cpu.get_state().registers.sp == 0xff);

    CHECK(bus.read(0x2) == 0);
    CHECK(bus.read(0x3) == 0);
}

} // namespace nese