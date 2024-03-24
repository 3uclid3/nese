set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nesesan")

set(INCLUDES
)

set(SOURCES
    "${DIR}/imgui/format_test.cpp"
    "${DIR}/menu/basic_menu_test.cpp"
)

add_executable(nesesan_test ${INCLUDES} ${SOURCES})

target_link_libraries(
    nesesan_test
    PRIVATE
        nese_project_options
        nese_project_warnings
        nese_catch2
        nesesan
)

target_include_interface_directories(nesesan_test "${CMAKE_CURRENT_SOURCE_DIR}")

# Set the folder for the project in Visual Studio
set_property(TARGET nesesan_test PROPERTY FOLDER "tests")

# use xml reporter if coverage is enabled
if(${ENABLE_COVERAGE})
  set(COVERAGE_ARGS REPORTER xml)
endif()

# automatically discover tests that are defined in catch based test files you can modify the tests
catch_discover_tests(nesesan_test ${COVERAGE_ARGS})
