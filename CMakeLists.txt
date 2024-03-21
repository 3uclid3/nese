cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

# Add project_options from https://github.com/aminya/project_options
# Change the version in the following URL to update the package (watch the releases of the repository for future updates)
set(PROJECT_OPTIONS_VERSION "v0.35.1")
FetchContent_Declare(_project_options URL https://github.com/aminya/project_options/archive/refs/tags/${PROJECT_OPTIONS_VERSION}.zip)
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

# install vcpkg dependencies: - should be called before defining project()
run_vcpkg(
    VCPKG_URL "https://github.com/microsoft/vcpkg.git"
    VCPKG_REV "10e052511428d6b0c7fcc63a139e8024bb146032"
)

# Set the project name and language
project(
    nese
    VERSION 0.0.1
    DESCRIPTION "NES Emulator"
    LANGUAGES CXX C
)

# Build Features
option(FEATURE_TESTS "Enable the tests" ON)

# vcpkg test feature
if(FEATURE_TESTS)
  list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

# Enable sanitizers and static analyzers when running the tests
if(FEATURE_TESTS)
  set(ENABLE_CLANG_TIDY "ENABLE_CLANG_TIDY")
  set(ENABLE_CPPCHECK "ENABLE_CPPCHECK")
  set(ENABLE_COVERAGE "ENABLE_COVERAGE")
  enable_testing()

  check_sanitizers_support(ENABLE_SANITIZER_ADDRESS
                           ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
                           ENABLE_SANITIZER_LEAK
                           ENABLE_SANITIZER_THREAD
                           ENABLE_SANITIZER_MEMORY)
endif()

# Initialize project_options variable related to this project
# This overwrites `project_options` and sets `project_warnings`
# uncomment to enable the options. Some of them accept one or more inputs:
project_options(
      PREFIX "nese"
      ENABLE_CACHE
      # ${ENABLE_CPPCHECK}
      # ${ENABLE_CLANG_TIDY}
      ENABLE_VS_ANALYSIS
      # ENABLE_INTERPROCEDURAL_OPTIMIZATION
      # ENABLE_NATIVE_OPTIMIZATION
      ${ENABLE_COVERAGE}
      ${ENABLE_SANITIZER_ADDRESS}
      ${ENABLE_SANITIZER_UNDEFINED_BEHAVIOR}
      # ${ENABLE_SANITIZER_THREAD}
      # ${ENABLE_SANITIZER_MEMORY}
      # ENABLE_CONTROL_FLOW_PROTECTION
      # ENABLE_STACK_PROTECTION
      # ENABLE_OVERFLOW_PROTECTION
      # ENABLE_ELF_PROTECTION
      # ENABLE_RUNTIME_SYMBOLS_RESOLUTION
      # ENABLE_COMPILE_COMMANDS_SYMLINK
      # ENABLE_PCH
      # PCH_HEADERS
      WARNINGS_AS_ERRORS
      # ENABLE_INCLUDE_WHAT_YOU_USE
      # ENABLE_GCC_ANALYZER
      # ENABLE_BUILD_WITH_TIME_TRACE
      # ENABLE_UNITY
      # LINKER "lld"
)

add_subdirectory("nese")

if(FEATURE_TESTS)
    add_subdirectory("tests")
endif()

package_project(
    TARGETS nese nese_project_options nese_project_warnings
)
