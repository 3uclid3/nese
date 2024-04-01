set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nesesan.debugger")

set(INCLUDES
    "${DIR}/cpu_state_view.hpp"
)
 
set(SOURCES
    "${DIR}/cpu_state_view.cpp"
)

add_library(nesesan.debugger ${INCLUDES} ${SOURCES})

target_link_libraries(
    nesesan.debugger
    PUBLIC
        nesesan
    PRIVATE
        nese.snapshot
        nese_project_options
        nese_project_warnings
)

target_include_interface_directories(nesesan.debugger "${CMAKE_CURRENT_SOURCE_DIR}")
