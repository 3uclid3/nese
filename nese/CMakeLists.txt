set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese")

set(INCLUDES
    "${DIR}/addr_mode.hpp"
    "${DIR}/basic_types.hpp"
    "${DIR}/cpu.hpp"
    "${DIR}/cpu_fmt.inl"
    "${DIR}/cpu_registers.hpp"
    "${DIR}/cpu_status.hpp"
    "${DIR}/cycle.hpp"
    "${DIR}/emulator.hpp"
    "${DIR}/log.hpp"
    "${DIR}/ram.hpp"
    "${DIR}/rom.hpp"
)

set(SOURCES
    "${DIR}/cpu.cpp"
    "${DIR}/emulator.cpp"
    "${DIR}/ram.cpp"
    "${DIR}/rom.cpp"
)

add_library(nese ${INCLUDES} ${SOURCES})

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
)

target_link_system_libraries(
    nese
    PRIVATE 
        fmt::fmt
        spdlog::spdlog
)
