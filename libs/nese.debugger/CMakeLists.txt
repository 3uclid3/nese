set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese.debugger")

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

add_library(nese.debugger ${INCLUDES} ${SOURCES})

target_link_libraries(
    nese.debugger
    PUBLIC
        nese.gui
    PRIVATE
        nese_project_options
        nese_project_warnings
)

target_include_interface_directories(nese.debugger "${CMAKE_CURRENT_SOURCE_DIR}")

# Set the folder for the project in Visual Studio
set_property(TARGET nese.debugger PROPERTY FOLDER "libs")
