set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nesesan.debugger")

set(INCLUDES
    "${DIR}/cpu_debugger.hpp"
    "${DIR}/cpu_debugger_view.hpp"
    "${DIR}/extension.hpp"
    "${DIR}/ram_view.hpp"
)
 
set(SOURCES
    "${DIR}/cpu_debugger.cpp"
    "${DIR}/cpu_debugger_view.cpp"
    "${DIR}/extension.cpp"
    "${DIR}/ram_view.cpp"
)

add_library(nesesan.debugger ${INCLUDES} ${SOURCES})

target_link_libraries(
    nesesan.debugger
    PUBLIC
        nese.debug
        nesesan
    PRIVATE
        nese_project_options
        nese_project_warnings
)

target_include_interface_directories(nesesan.debugger "${CMAKE_CURRENT_SOURCE_DIR}")
