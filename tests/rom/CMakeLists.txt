# config
set(NESTEST_ROM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/nestest/nestest.nes")

configure_file("test_config.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/nestest/test_config.hpp")

# exe
add_executable(
    nestest.romtest
    "./nestest/nestest_tests.cpp"
)

target_link_libraries(
    nestest.romtest
    PRIVATE
        nese_project_options
        nese_project_warnings
        nese_catch2
        nese
)

target_disable_static_analysis(nestest.romtest)

# Set the folder for the project in Visual Studio
set_property(TARGET nestest.romtest PROPERTY FOLDER "tests/rom")

# automatically discover tests that are defined in catch based test files you can modify the tests
catch_discover_tests(nestest.romtest ${COVERAGE_ARGS})
