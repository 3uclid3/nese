add_executable(
    nese.app

    "./glad/glad.c"
    "./glad/glad.h"

    "./KHR/khrplatform.h"
 
    "./main.cpp"
)

target_link_libraries(
    nese.app
    PRIVATE
        nese
        nese.gui
        nese.debugger
        nese_project_options
        nese_project_warnings
)

target_disable_static_analysis(nese.app)

target_include_interface_directories(nese.app "${CMAKE_CURRENT_SOURCE_DIR}")

target_find_dependencies(
    nese.app
    PRIVATE_CONFIG
        imgui
        glad
        glfw3
)

target_link_system_libraries(
    nese.app
    PRIVATE 
        imgui::imgui
        glad::glad
        glfw
)
 
add_custom_command(
    TARGET nese.app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/fonts/fa-regular-400.ttf" "${PROJECT_SOURCE_DIR}/fonts/fa-solid-900.ttf" "${CMAKE_CURRENT_BINARY_DIR}/"
    COMMENT "Copying fonts to runtime directory from ${PROJECT_SOURCE_DIR}/fonts/ to ${CMAKE_CURRENT_BINARY_DIR}/"
)

set_target_properties(nese.app PROPERTIES OUTPUT_NAME nese)
set_property(TARGET nese.app PROPERTY FOLDER "app")
