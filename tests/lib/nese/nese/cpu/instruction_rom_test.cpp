#include <catch2/catch_test_macros.hpp>

#include <fmt/format.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/state_mock.hpp>
#include <nese/memory/rom.hpp>
#include <nese/test_config.hpp>
#include <nese/utility/log.hpp>

namespace nese::cpu {

struct fixture
{
    void run_rom(std::string_view rom_name, word_t end_pc)
    {
        std::shared_ptr<spdlog::logger> nintendulator_logger = spdlog::basic_logger_st("nintendulator", fmt::format("cpu_nintendulator_{}.log", rom_name), true);
        nintendulator_logger->set_level(spdlog::level::trace);
        nintendulator_logger->set_pattern("%v");

        const std::string path = fmt::format("{}/{}", test_roms_path, rom_name);
        const memory::rom rom = memory::rom::from_file(path.c_str());

        state.registers.pc = rom.get_prg().size() == 0x4000 ? 0xc000 : 0x8000; // TODO Mapper

        state.owned_memory.set_bytes(0x8000, rom.get_prg().data(), rom.get_prg().size());

        if (rom.get_prg().size() == 0x4000)
        {
            // "map" 0xC000 to 0x8000
            state.owned_memory.set_bytes(0xc000, rom.get_prg().data(), rom.get_prg().size());
        }

        bool last_instruction_succeeded = true;
        while (last_instruction_succeeded && state.registers.pc != end_pc)
        {
            previous_state = state;

            nintendulator_logger->trace("{}", state);

            const auto opcode = state.owned_memory.get_byte(state.registers.pc++);
            last_instruction_succeeded = instruction::execute(opcode, state);
        }

        if (state.registers.pc != end_pc)
        {
            const auto message =
                fmt::format("Mismatch PC: expected 0x{:04X}, actual 0x{:04X}\nPrevious CPU State:\n{}\nCurrent CPU State:\n{}",
                            end_pc,
                            state.registers.pc,
                            previous_state,
                            state);
            FAIL(message.c_str());
        }
    }

    state_mock previous_state{};
    state_mock state{};
};

TEST_CASE_METHOD(fixture, "nestest", "[cpu][instruction][!mayfail]")
{
    run_rom("nestest.nes", 0x0005);
    
    CHECK(state.registers.s == 0xff);
    
    CHECK(state.owned_memory.get_byte(0x2) == 0);
    CHECK(state.owned_memory.get_byte(0x3) == 0);
}

} // namespace nese::cpu