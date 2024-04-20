set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese")

set(INCLUDES
    "${DIR}/cartridge/cartridge_mapper.hpp"
    "${DIR}/cartridge/cartridge_mapper_factory.hpp"
    "${DIR}/cartridge/nrom_cartridge_mapper.hpp"
    "${DIR}/cpu/cpu_addr_mode.hpp"
    "${DIR}/cpu/cpu_opcode.hpp"
    "${DIR}/cpu/cpu_opcode_enum.hpp"
    "${DIR}/cpu/cpu_registers.hpp"
    "${DIR}/cpu/cpu_stack_offset.hpp"
    "${DIR}/cpu/cpu_state.hpp"
    "${DIR}/cpu/cpu_status.hpp"
    "${DIR}/utility/details/crc32_table.hpp"
    "${DIR}/utility/assert.hpp"
    "${DIR}/utility/crc32.hpp"
    "${DIR}/utility/format.hpp"
    "${DIR}/utility/log.hpp"
    "${DIR}/utility/name_of.hpp"
    "${DIR}/utility/nintendulator.hpp"
    "${DIR}/utility/passkey.hpp"
    "${DIR}/basic_types.hpp"
    "${DIR}/bus.hpp"
    "${DIR}/cartridge.hpp"
    "${DIR}/cpu.hpp"
    "${DIR}/cpu.inl"
    "${DIR}/emulator.hpp"
    "${DIR}/ppu.hpp"
    "${DIR}/ppu.inl"
)

set(SOURCES
    "${DIR}/cartridge/cartridge_mapper_factory.cpp"
    "${DIR}/cartridge/nrom_cartridge_mapper.cpp"
    "${DIR}/utility/format.cpp"
    "${DIR}/utility/nintendulator.cpp"
    "${DIR}/bus.cpp"
    "${DIR}/cartridge.cpp"
    "${DIR}/emulator.cpp"
)

set(VISUALIZERS
    "nese.natvis"
)

add_library(nese ${INCLUDES} ${SOURCES} ${VISUALIZERS})

target_link_libraries(
    nese
    PRIVATE 
        nese_project_options
        nese_project_warnings
)

target_include_interface_directories(nese "${CMAKE_CURRENT_SOURCE_DIR}")

target_find_dependencies(
    nese
    PRIVATE_CONFIG
        fmt
        spdlog
        magic_enum
)

target_link_system_libraries(
    nese
    PUBLIC
        spdlog::spdlog
        fmt::fmt
        magic_enum::magic_enum
)
