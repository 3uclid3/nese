set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese")

set(TEST_ROMS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/test_roms")

configure_file("test_config.hpp.in" "${DIR}/test_config.hpp")

set(INCLUDES
    "${DIR}/cpu/instruction/catch_generators.hpp"
    "${DIR}/cpu/instruction/fixture.hpp"
    "${DIR}/cpu/state_mock.hpp"
    "${DIR}/test_config.hpp"
)

set(SOURCES
    "${DIR}/cpu/instruction/branch_test.cpp"
    "${DIR}/cpu/instruction/in_test.cpp"
    "${DIR}/cpu/instruction/jmp_test.cpp"
    "${DIR}/cpu/instruction/jsr_test.cpp"
    "${DIR}/cpu/instruction/ld_test.cpp"
    "${DIR}/cpu/instruction/nop_test.cpp"
    "${DIR}/cpu/instruction/sec_test.cpp"
    "${DIR}/cpu/instruction/st_test.cpp"
    "${DIR}/cpu/instruction_rom_test.cpp"
    "${DIR}/utility/format_test.cpp"
)

add_executable(nese_test ${INCLUDES} ${SOURCES})

target_link_libraries(
    nese_test
    PRIVATE
        nese_project_options
        nese_project_warnings
        nese_catch2
        nese
)

target_disable_static_analysis(nese_test)

target_include_interface_directories(nese_test "${CMAKE_CURRENT_SOURCE_DIR}")

# Set the folder for the project in Visual Studio
set_property(TARGET nese_test PROPERTY FOLDER "tests")

# use xml reporter if coverage is enabled
if(${ENABLE_COVERAGE})
  set(COVERAGE_ARGS REPORTER xml)
endif()

# automatically discover tests that are defined in catch based test files you can modify the tests
catch_discover_tests(nese_test ${COVERAGE_ARGS})
