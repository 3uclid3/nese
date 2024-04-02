#include <catch2/catch_test_macros.hpp>

#include <spdlog/sinks/basic_file_sink.h>

#include <nese/cpu/instruction/execute.hpp>
#include <nese/cpu/instruction/execute_context.hpp>
#include <nese/cpu/state.hpp>
#include <nese/cycle.hpp>
#include <nese/memory/mapper.hpp>
#include <nese/memory/rom.hpp>
#include <nese/utility/nintendulator.hpp>
#include <nese/test_config.hpp>
#include <nese/utility/format.hpp>
#include <nese/utility/log.hpp>

namespace nese::cpu {

struct fixture
{
    void run_rom(std::string_view rom_name, word_t end_pc, cpu_cycle_t cycle = {})
    {
        std::shared_ptr<spdlog::logger> nintendulator_logger = spdlog::basic_logger_st("nintendulator", fmt::format("cpu_nintendulator_{}.log", rom_name), true);
        nintendulator_logger->set_level(spdlog::level::trace);
        nintendulator_logger->set_pattern("%v");

        const char* path = nese::format("{}/{}", test_roms_path, rom_name);
        const memory::rom rom = memory::rom::from_file(path);

        state.registers.pc = rom.get_prg().size() == 0x4000 ? 0xc000 : 0x8000; // TODO Mapper
        state.cycle = cycle;

        memory.set_bytes(0x8000, rom.get_prg().data(), rom.get_prg().size());

        if (rom.get_prg().size() == 0x4000)
        {
            // "map" 0xC000 to 0x8000
            memory.set_bytes(0xc000, rom.get_prg().data(), rom.get_prg().size());
        }

        bool last_instruction_succeeded = true;
        while (last_instruction_succeeded && state.registers.pc != end_pc)
        {
            previous_state = state;
            previous_memory = memory;

            nintendulator_logger->trace(nintendulator::format(state, memory));

            const instruction::execute_context context(state, memory);
            last_instruction_succeeded = instruction::execute(context);
        }

        if (state.registers.pc != end_pc)
        {
            const auto message =
                fmt::format("Mismatch PC: expected 0x{:04X}, actual 0x{:04X}\nPrevious State:\n{}\nCurrent State:\n{}",
                            end_pc,
                            state.registers.pc,
                            nintendulator::format(previous_state, previous_memory),
                            nintendulator::format(state, memory));
            FAIL(message.c_str());
        }
    }

    cpu::state previous_state{};
    memory::mapper previous_memory{};

    cpu::state state{};
    memory::mapper memory{};
};

TEST_CASE_METHOD(fixture, "nestest", "[cpu][instruction][!mayfail]")
{
    run_rom("nestest.nes", 0x0005, cpu_cycle_t{7});

    CHECK(state.registers.s == 0xff);

    CHECK(memory.get_byte(0x2) == 0);
    CHECK(memory.get_byte(0x3) == 0);
}

} // namespace nese::cpu