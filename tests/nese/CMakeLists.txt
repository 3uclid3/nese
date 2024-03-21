set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese")

set(TEST_ROMS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/test_roms")

configure_file("test_config.hpp.in" "${DIR}/test_config.hpp")

set(INCLUDES
    "${DIR}/cpu_mock.hpp"
    "${DIR}/test_config.hpp"
)

set(SOURCES
    "${DIR}/cpu_fmt_test.cpp"
    "${DIR}/cpu_test.cpp"
)

add_executable(nese_test ${INCLUDES} ${SOURCES})

target_link_libraries(
    nese_test
    PRIVATE
        nese
        nese_project_options
        nese_project_warnings
)

target_find_dependencies(
    nese_test
    PRIVATE_CONFIG
        fmt
        spdlog
        Catch2
)

target_link_system_libraries(
    nese_test
    PRIVATE 
        fmt::fmt
        spdlog::spdlog
        Catch2::Catch2WithMain
)

target_include_interface_directories(nese_test "${CMAKE_CURRENT_SOURCE_DIR}")

# Set the folder for the project in Visual Studio
set_property(TARGET nese_test PROPERTY FOLDER "tests")