set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese")

set(INCLUDES
    "${DIR}/cpu/addr_mode.hpp"
    "${DIR}/cpu/instruction.hpp"
    "${DIR}/cpu/registers.hpp"
    "${DIR}/cpu/state.hpp"
    "${DIR}/cpu/state_fmt.inl"
    "${DIR}/cpu/status_flag.hpp"
    "${DIR}/memory/mapper.hpp"
    "${DIR}/memory/ram.hpp"
    "${DIR}/memory/rom.hpp"
    "${DIR}/utility/assert.hpp"
    "${DIR}/utility/format.hpp"
    "${DIR}/utility/log.hpp"
    "${DIR}/basic_types.hpp"
    "${DIR}/cycle.hpp"
)

set(SOURCES
    "${DIR}/cpu/instruction.cpp"
    "${DIR}/memory/mapper.cpp"
    "${DIR}/memory/ram.cpp"
    "${DIR}/memory/rom.cpp"
    "${DIR}/utility/format.cpp"
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
