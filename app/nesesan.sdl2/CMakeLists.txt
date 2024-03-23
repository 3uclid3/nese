set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nesesan.sdl2")

set(INCLUDES
)

set(SOURCES
    "${DIR}/main.cpp"
)

add_executable(nesesan.sdl2 ${INCLUDES} ${SOURCES})

set_target_properties(nesesan.sdl2 PROPERTIES
    OUTPUT_NAME nesesan
)

target_link_libraries(
    nesesan.sdl2
    PRIVATE
        nesesan
        nese_project_options
        nese_project_warnings
)

target_include_interface_directories(nesesan.sdl2 "${CMAKE_CURRENT_SOURCE_DIR}")

target_find_dependencies(
    nesesan.sdl2
    PRIVATE_CONFIG
        imgui
        sdl2
)

target_link_system_libraries(
    nesesan.sdl2
    PRIVATE 
        imgui::imgui
        SDL2::SDL2
        SDL2::SDL2main
)
