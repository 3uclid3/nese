#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/processor_mock.hpp>
#include <nese/memory/ram.hpp>
#include <nese/memory/rom.hpp>
#include <nese/test_config.hpp>

namespace nese::cpu {

void run_rom(processor& cpu, memory::ram& ram, std::string_view rom_name)
{
    const std::string path = fmt::format("{}/{}", test_roms_path, rom_name);

    memory::rom rom = memory::rom::from_file(path.c_str());

    cpu.power_on();
    ram.power_on();

    cpu.set_code_addr(rom.get_prg().size() == 0x4000 ? 0xc000 : 0x8000); // TODO Mapper

    ram.set_bytes(0x8000, rom.get_prg().data(), rom.get_prg().size());

    if (rom.get_prg().size() == 0x4000)
    {
        // "map" 0xC000 to 0x8000
        ram.set_bytes(0xc000, rom.get_prg().data(), rom.get_prg().size());
    }

    // cpu.debug_stop_at(0xC75D);

    while (!cpu.has_stop_requested())
    {
        cpu.step();
    }
}

TEST_CASE("cpu - nestest")
{
    processor_mock cpu;

    // run_rom(cpu, cpu.ram, "nestest.nes");

    // CHECK(cpu.get_registers().pc == 0x0005);
    // CHECK(cpu.get_registers().s == 0xff);
    
    // CHECK(cpu.ram.get_byte(0x2) == 0);
    // CHECK(cpu.ram.get_byte(0x3) == 0);
}

} // namespace nese