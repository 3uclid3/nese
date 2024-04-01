set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nesesan.opengl")

set(INCLUDES
    "${DIR}/glad/glad.h"
    "${DIR}/KHR/khrplatform.h"
)

set(SOURCES
    "${DIR}/glad/glad.c"
    "${DIR}/main.cpp"
)

add_executable(nesesan.opengl ${INCLUDES} ${SOURCES})

set_target_properties(nesesan.opengl PROPERTIES
    OUTPUT_NAME nesesan
)

target_link_libraries(
    nesesan.opengl
    PRIVATE
        nesesan
        nesesan.debugger
        nese_project_options
        nese_project_warnings
)

target_disable_static_analysis(nesesan.opengl)

target_include_interface_directories(nesesan.opengl "${CMAKE_CURRENT_SOURCE_DIR}")

target_find_dependencies(
    nesesan.opengl
    PRIVATE_CONFIG
        imgui
        glad
        glfw3
)

target_link_system_libraries(
    nesesan.opengl
    PRIVATE 
        imgui::imgui
        glad::glad
        glfw
)
 
#add_custom_target(nesesan_copy_fonts ALL
add_custom_command(TARGET nesesan.opengl POST_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  "${PROJECT_SOURCE_DIR}/fonts/fa-regular-400.ttf" "${PROJECT_SOURCE_DIR}/fonts/fa-solid-900.ttf" "${CMAKE_CURRENT_BINARY_DIR}/"
                  COMMENT "Copying fonts to runtime directory from ${PROJECT_SOURCE_DIR}/fonts/ to ${CMAKE_CURRENT_BINARY_DIR}/")

# add_dependencies(nesesan.opengl nesesan_copy_fonts)
