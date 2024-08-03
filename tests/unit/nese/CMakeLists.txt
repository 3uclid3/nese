add_executable(
    nese.unittest
    "./nese/utility/name_of_test.cpp"
    "./nese/utility/format_test.cpp"
    "./nese/utility/crc32_test.cpp"
    "./nese/graphic/color_test.cpp"
    "./nese/cpu_test.cpp"
    "./nese/cpu_fixture.cpp"
    "./nese/cpu_fixture.hpp"
    "./nese/cpu_bus_mock.hpp"
)

target_link_libraries(
    nese.unittest
    PRIVATE
        nese_project_options
        nese_project_warnings
        nese_catch2
        nese
)

target_disable_static_analysis(nese.unittest)
target_include_interface_directories(nese.unittest "${CMAKE_CURRENT_SOURCE_DIR}")

# Set the folder for the project in Visual Studio
set_property(TARGET nese.unittest PROPERTY FOLDER "tests/unit")

# use xml reporter if coverage is enabled
if(${ENABLE_COVERAGE})
  set(COVERAGE_ARGS REPORTER xml)
endif()

# automatically discover tests that are defined in catch based test files you can modify the tests
catch_discover_tests(nese.unittest ${COVERAGE_ARGS})
