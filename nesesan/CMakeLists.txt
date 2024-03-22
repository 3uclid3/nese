set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nesesan")

set(INCLUDES
    "${DIR}/imgui_impl_sdlrenderer2.h"
    "${DIR}/imgui_impl_sdl2.h"
)

set(SOURCES
    "${DIR}/imgui_impl_sdlrenderer2.cpp"
    "${DIR}/imgui_impl_sdl2.cpp"
    "${DIR}/main.cpp"
)

add_executable(nesesan ${INCLUDES} ${SOURCES})

target_link_libraries(
    nesesan
    PRIVATE 
        nese_project_options
        nese_project_warnings
)

target_include_interface_directories(nesesan "${CMAKE_CURRENT_SOURCE_DIR}")

target_find_dependencies(
    nesesan
    PRIVATE_CONFIG
        fmt
        imgui
        sdl2
        spdlog
)

target_link_system_libraries(
    nesesan
    PRIVATE 
        fmt::fmt
        imgui::imgui
        SDL2::SDL2
        SDL2::SDL2main
        spdlog::spdlog
)