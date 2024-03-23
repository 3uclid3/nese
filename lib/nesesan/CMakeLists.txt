set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nesesan")

set(INCLUDES
    "${DIR}/application.hpp"
)

set(SOURCES
    "${DIR}/application.cpp"
)

add_library(nesesan ${INCLUDES} ${SOURCES})

target_link_libraries(
    nesesan
    PUBLIC
        nese
    PRIVATE
        nese_project_options
        nese_project_warnings
)

target_include_interface_directories(nesesan "${CMAKE_CURRENT_SOURCE_DIR}")

target_find_dependencies(
    nesesan
    PRIVATE_CONFIG
        imgui
)

target_link_system_libraries(
    nesesan
    PRIVATE 
        imgui::imgui
)