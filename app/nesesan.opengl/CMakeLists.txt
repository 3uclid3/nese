set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nesesan.opengl")

set(INCLUDES
    "${DIR}/glad/glad.h"
    "${DIR}/KHR/khrplatform.h"
)

set(SOURCES
    "${DIR}/glad/glad.c"
    "${DIR}/main.cpp"
)

add_executable(nese.opengl ${INCLUDES} ${SOURCES})

set_target_properties(nese.opengl PROPERTIES
    OUTPUT_NAME nese
)

target_link_libraries(
    nese.opengl
    PRIVATE
        nese
        nese.debugger
        nese_project_options
        nese_project_warnings
)

target_disable_static_analysis(nese.opengl)

target_include_interface_directories(nese.opengl "${CMAKE_CURRENT_SOURCE_DIR}")

target_find_dependencies(
    nese.opengl
    PRIVATE_CONFIG
        imgui
        glad
        glfw3
)

target_link_system_libraries(
    nese.opengl
    PRIVATE 
        imgui::imgui
        glad::glad
        glfw
)
 
add_custom_command(TARGET nese.opengl POST_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  "${PROJECT_SOURCE_DIR}/fonts/fa-regular-400.ttf" "${PROJECT_SOURCE_DIR}/fonts/fa-solid-900.ttf" "${CMAKE_CURRENT_BINARY_DIR}/"
                  COMMENT "Copying fonts to runtime directory from ${PROJECT_SOURCE_DIR}/fonts/ to ${CMAKE_CURRENT_BINARY_DIR}/")
