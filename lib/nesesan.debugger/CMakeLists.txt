set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nesesan.debugger")

set(INCLUDES
    "${DIR}/disassembly_view.hpp"
    "${DIR}/cpu_state_view.hpp"
    "${DIR}/debug_control_view.hpp"
)
 
set(SOURCES
    "${DIR}/disassembly_view.cpp"
    "${DIR}/cpu_state_view.cpp"
    "${DIR}/debug_control_view.cpp"
)

add_library(nesesan.debugger ${INCLUDES} ${SOURCES})

target_link_libraries(
    nesesan.debugger
    PUBLIC
        nesesan
    PRIVATE
        nese_project_options
        nese_project_warnings
)

target_include_interface_directories(nesesan.debugger "${CMAKE_CURRENT_SOURCE_DIR}")
