#include <catch2/catch_test_macros.hpp>

#include <spdlog/sinks/basic_file_sink.h>

#include <nese/bus.hpp>
#include <nese/test_config.hpp>
#include <nese/utility/format.hpp>
#include <nese/utility/hex.hpp>
#include <nese/utility/log.hpp>
#include <nese/utility/nintendulator.hpp>

namespace nese {

TEST_CASE("nestest", "[cpu][instruction]")
{
    constexpr cpu_cycle_t start_cycle = cpu_cycle_t(7);
    constexpr addr_x start_pc = 0xC000;
    constexpr addr_x end_pc = 0x0005;

    bus bus;

    std::shared_ptr<spdlog::logger> nintendulator_logger = spdlog::basic_logger_st("nintendulator", "cpu_nintendulator_nestest.log", true);
    nintendulator_logger->set_level(spdlog::level::trace);
    nintendulator_logger->set_pattern("%v");

    const char* filepath = format("{}/nestest.nes", test_roms_path);
    bus.cartridge = cartridge::from_file(filepath);
    bus.cpu.get_state().registers.pc = start_pc;
    bus.cpu.get_state().cycle = start_cycle;
    bus.ppu._cycle = ppu_cycle_t(21);
    bus.ppu._scanline_cycle = ppu_cycle_t(21);

    auto cycle = cycle_t(21);
    bool last_instruction_succeeded = true;
    while (last_instruction_succeeded && bus.cpu.get_state().registers.pc != end_pc)
    {
        if (cycle == bus.cpu.get_state().cycle)
        {
            nintendulator_logger->trace(nintendulator::format(bus));
        }

        ++cycle;

        bus.ppu.step(cycle);
        last_instruction_succeeded = bus.cpu.step(cycle);
    }

    REQUIRE(bus.cpu.get_state().registers.pc == end_pc);

    CHECK(bus.cpu.get_state().registers.sp == 0xff);

    CHECK(bus.read(0x2) == 0);
    CHECK(bus.read(0x3) == 0);
}

} // namespace nese