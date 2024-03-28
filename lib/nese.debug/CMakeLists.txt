set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese.debug")

set(INCLUDES
    "${DIR}/cpu/snapshot.hpp"
    "${DIR}/cpu/snapshotter.hpp"
)

set(SOURCES
    "${DIR}/cpu/snapshotter.cpp"
)

add_library(nese.debug ${INCLUDES} ${SOURCES})

target_link_libraries(
    nese.debug
    PUBLIC
        nese
    PRIVATE
        nese_project_options
        nese_project_warnings
)

target_include_interface_directories(nese.debug "${CMAKE_CURRENT_SOURCE_DIR}")
