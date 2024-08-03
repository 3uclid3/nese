set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese.gui")

set(INCLUDES
    "${DIR}/menu/basic_menu.hpp"
    "${DIR}/menu/basic_submenu_menu_item.hpp"
    "${DIR}/menu/callback_menu_item.hpp"
    "${DIR}/menu/main_menu.hpp"
    "${DIR}/menu/menu.hpp"
    "${DIR}/view/game_view.hpp"
    "${DIR}/view/view.hpp"
    "${DIR}/view/view_draw_context.hpp"
    "${DIR}/view/view_registry.hpp"
    "${DIR}/view/window_view.hpp"
    "${DIR}/application.hpp"
    "${DIR}/icons.hpp"
    "${DIR}/imgui.hpp"
    "${DIR}/texture.hpp"
)

set(SOURCES
    "${DIR}/menu/basic_submenu_menu_item.cpp"
    "${DIR}/menu/callback_menu_item.cpp"
    "${DIR}/menu/main_menu.cpp"
    "${DIR}/menu/menu.cpp"
    "${DIR}/view/game_view.cpp"
    "${DIR}/view/view.cpp"
    "${DIR}/view/window_view.cpp"
    "${DIR}/application.cpp"
    "${DIR}/texture.cpp"
)

add_library(nese.gui ${INCLUDES} ${SOURCES})

target_link_libraries(
    nese.gui
    PUBLIC
        nese
    PRIVATE
        nese_project_options
        nese_project_warnings
)

target_include_interface_directories(nese.gui "${CMAKE_CURRENT_SOURCE_DIR}")

target_find_dependencies(
    nese.gui
    PRIVATE_CONFIG
        imgui
)

target_link_system_libraries(
    nese.gui
    PRIVATE 
        imgui::imgui
)

# Set the folder for the project in Visual Studio
set_property(TARGET nese.gui PROPERTY FOLDER "libs")
