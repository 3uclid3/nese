set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese")

set(INCLUDES
    "${DIR}/cpu/instruction/execute.hpp"
    "${DIR}/cpu/instruction/execute_context.hpp"
    "${DIR}/cpu/instruction/opcode.hpp"
    "${DIR}/cpu/instruction/opcode_enum.hpp"
    "${DIR}/cpu/instruction/operand.hpp"
    "${DIR}/cpu/addr_mode.hpp"
    "${DIR}/cpu/cpu_addr_mode.hpp"
    "${DIR}/cpu/cpu_opcode.hpp"
    "${DIR}/cpu/cpu_opcode_enum.hpp"
    "${DIR}/cpu/cpu_registers.hpp"
    "${DIR}/cpu/cpu_stack_offset.hpp"
    "${DIR}/cpu/cpu_state.hpp"
    "${DIR}/cpu/cpu_status.hpp"
    "${DIR}/cpu/registers.hpp"
    "${DIR}/cpu/stack.hpp"
    "${DIR}/cpu/state.hpp"
    "${DIR}/cpu/status_flag.hpp"
    "${DIR}/ppu/control.hpp"
    "${DIR}/ppu/state.hpp"
    "${DIR}/memory/mapper.hpp"
    "${DIR}/memory/ram.hpp"
    "${DIR}/memory/rom.hpp"
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
    "${DIR}/cpu.hpp"
    "${DIR}/cpu.inl"
    "${DIR}/cycle.hpp"
    "${DIR}/emulator.hpp"
)

set(SOURCES
    "${DIR}/cpu/instruction/execute.cpp"
    "${DIR}/memory/mapper.cpp"
    "${DIR}/memory/rom.cpp"
    "${DIR}/utility/format.cpp"
    "${DIR}/utility/nintendulator.cpp"
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
