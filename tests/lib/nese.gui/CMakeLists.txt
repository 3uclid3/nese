set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese.gui")

set(INCLUDES
)

set(SOURCES
    "${DIR}/menu/basic_menu_test.cpp"
)

add_executable(nese.gui.unittest ${INCLUDES} ${SOURCES})

target_link_libraries(
    nese.gui.unittest
    PRIVATE
        nese_project_options
        nese_project_warnings
        nese_catch2
        nese.gui
)

target_disable_static_analysis(nese.gui.unittest)

target_include_interface_directories(nese.gui.unittest "${CMAKE_CURRENT_SOURCE_DIR}")

# Set the folder for the project in Visual Studio
set_property(TARGET nese.gui.unittest PROPERTY FOLDER "tests")

# use xml reporter if coverage is enabled
if(${ENABLE_COVERAGE})
  set(COVERAGE_ARGS REPORTER xml)
endif()

# automatically discover tests that are defined in catch based test files you can modify the tests
catch_discover_tests(nese.gui.unittest ${COVERAGE_ARGS})
