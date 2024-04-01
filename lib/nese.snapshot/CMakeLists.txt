set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese.snapshot")

set(INCLUDES
    "${DIR}/snapshot.hpp"
    "${DIR}/snapshotter.hpp"
)

set(SOURCES
    "${DIR}/snapshotter.cpp"
)

add_library(nese.snapshot ${INCLUDES} ${SOURCES})

target_link_libraries(
    nese.snapshot
    PUBLIC
        nese
    PRIVATE
        nese_project_options
        nese_project_warnings
)

target_include_interface_directories(nese.snapshot "${CMAKE_CURRENT_SOURCE_DIR}")
