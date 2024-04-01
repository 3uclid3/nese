set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nesesan")

set(INCLUDES
    "${DIR}/menu/basic_menu.hpp"
    "${DIR}/menu/basic_submenu_menu_item.hpp"
    "${DIR}/menu/callback_menu_item.hpp"
    "${DIR}/menu/main_menu.hpp"
    "${DIR}/menu/menu.hpp"
    "${DIR}/view/view.hpp"
    "${DIR}/view/view_draw_context.hpp"
    "${DIR}/view/view_registry.hpp"
    "${DIR}/view/window_view.hpp"
    "${DIR}/application.hpp"
    "${DIR}/icons.hpp"
    "${DIR}/imgui.hpp"
)

set(SOURCES
    "${DIR}/menu/basic_submenu_menu_item.cpp"
    "${DIR}/menu/callback_menu_item.cpp"
    "${DIR}/menu/main_menu.cpp"
    "${DIR}/menu/menu.cpp"
    "${DIR}/view/view.cpp"
    "${DIR}/view/view_registry.cpp"
    "${DIR}/view/window_view.cpp"
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
