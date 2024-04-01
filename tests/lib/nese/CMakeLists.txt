set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/nese")

set(TEST_ROMS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/test_roms")

configure_file("test_config.hpp.in" "${DIR}/test_config.hpp")

set(INCLUDES
    "${DIR}/cpu/instruction/fixture/branch_fixture.hpp"
    "${DIR}/cpu/instruction/fixture/execute_fixture.hpp"
    "${DIR}/cpu/instruction/fixture/increment_fixture.hpp"
    "${DIR}/cpu/instruction/fixture/load_fixture.hpp"
    "${DIR}/cpu/instruction/fixture/status_flag_fixture.hpp"
    "${DIR}/cpu/instruction/fixture/store_fixture.hpp"
    "${DIR}/test_config.hpp"
)

set(SOURCES
    "${DIR}/cpu/instruction/fixture/branch_fixture.cpp"
    "${DIR}/cpu/instruction/fixture/execute_fixture.cpp"
    "${DIR}/cpu/instruction/fixture/increment_fixture.cpp"
    "${DIR}/cpu/instruction/fixture/load_fixture.cpp"
    "${DIR}/cpu/instruction/fixture/status_flag_fixture.cpp"
    "${DIR}/cpu/instruction/fixture/store_fixture.cpp"
    "${DIR}/cpu/instruction/bcc_test.cpp"
    "${DIR}/cpu/instruction/bcs_test.cpp"
    "${DIR}/cpu/instruction/beq_test.cpp"
    "${DIR}/cpu/instruction/bit_test.cpp"
    "${DIR}/cpu/instruction/bne_test.cpp"
    "${DIR}/cpu/instruction/bpl_test.cpp"
    "${DIR}/cpu/instruction/bvc_test.cpp"
    "${DIR}/cpu/instruction/bvs_test.cpp"
    "${DIR}/cpu/instruction/clc_test.cpp"
    "${DIR}/cpu/instruction/inx_test.cpp"
    "${DIR}/cpu/instruction/iny_test.cpp"
    "${DIR}/cpu/instruction/jmp_test.cpp"
    "${DIR}/cpu/instruction/jsr_test.cpp"
    "${DIR}/cpu/instruction/lda_test.cpp"
    "${DIR}/cpu/instruction/ldx_test.cpp"
    "${DIR}/cpu/instruction/ldy_test.cpp"
    "${DIR}/cpu/instruction/nop_test.cpp"
    "${DIR}/cpu/instruction/opcode_test.cpp"
    "${DIR}/cpu/instruction/php_test.cpp"
    "${DIR}/cpu/instruction/pla_test.cpp"
    "${DIR}/cpu/instruction/plp_test.cpp"
    "${DIR}/cpu/instruction/rti_test.cpp"
    "${DIR}/cpu/instruction/rts_test.cpp"
    "${DIR}/cpu/instruction/sec_test.cpp"
    "${DIR}/cpu/instruction/sed_test.cpp"
    "${DIR}/cpu/instruction/sei_test.cpp"
    "${DIR}/cpu/instruction/stx_test.cpp"
    "${DIR}/cpu/instruction/sty_test.cpp"
    "${DIR}/cpu/instruction_rom_test.cpp"
    "${DIR}/utility/format_test.cpp"
    "${DIR}/utility/crc32_test.cpp"
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
