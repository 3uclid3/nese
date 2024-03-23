set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nesesan.sdl2")

set(INCLUDES
    "${DIR}/imgui_impl_sdlrenderer2.h"
    "${DIR}/imgui_impl_sdl2.h"
)

set(SOURCES
    "${DIR}/imgui_impl_sdlrenderer2.cpp"
    "${DIR}/imgui_impl_sdl2.cpp"
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
        fmt
        imgui
        sdl2
        spdlog
)

target_link_system_libraries(
    nesesan.sdl2
    PRIVATE 
        fmt::fmt
        imgui::imgui
        SDL2::SDL2
        SDL2::SDL2main
        spdlog::spdlog
)
