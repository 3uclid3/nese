set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese")

set(INCLUDES
    "${DIR}/cpu/addr_mode.hpp"
    "${DIR}/cpu/processor.hpp"
    "${DIR}/cpu/processor_fmt.inl"
    "${DIR}/cpu/registers.hpp"
    "${DIR}/cpu/status_flag.hpp"
    "${DIR}/memory/ram.hpp"
    "${DIR}/memory/rom.hpp"
    "${DIR}/utility/assert.hpp"
    "${DIR}/utility/log.hpp"
    "${DIR}/basic_types.hpp"
    "${DIR}/cycle.hpp"
    "${DIR}/emulator.hpp"
)

set(SOURCES
    "${DIR}/cpu/processor.cpp"
    "${DIR}/memory/ram.cpp"
    "${DIR}/memory/rom.cpp"
    "${DIR}/emulator.cpp"
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
    PUBLIC
        spdlog::spdlog
        fmt::fmt
)
