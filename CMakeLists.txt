cmake_minimum_required(VERSION 3.25)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
cmake_policy(SET CMP0135 NEW)

# Add project_options from https://github.com/aminya/project_options
set(PROJECT_OPTIONS_VERSION "v0.35.1")
FetchContent_Declare(_project_options URL https://github.com/aminya/project_options/archive/refs/tags/${PROJECT_OPTIONS_VERSION}.zip)
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

# Build Features
option(FEATURE_TESTS "Enable the tests" ON)

# vcpkg test feature
if(FEATURE_TESTS)
  list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

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

# Enable sanitizers and static analyzers when running the tests
if(FEATURE_TESTS)
    set(ENABLE_CLANG_TIDY "ENABLE_CLANG_TIDY")
    set(ENABLE_CPPCHECK "ENABLE_CPPCHECK")
    set(ENABLE_COVERAGE "ENABLE_COVERAGE")
    
    check_sanitizers_support(ENABLE_SANITIZER_ADDRESS
                             ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
                             ENABLE_SANITIZER_LEAK
                             ENABLE_SANITIZER_THREAD
                             ENABLE_SANITIZER_MEMORY)
    
    enable_testing()
    
    # generate a main function for the test executable
    add_library(nese_catch2 INTERFACE)
    target_find_dependencies(nese_catch2 INTERFACE_CONFIG Catch2)
    target_link_libraries(nese_catch2 INTERFACE Catch2::Catch2 Catch2::Catch2WithMain)
    target_compile_definitions(nese_catch2 INTERFACE DO_NOT_USE_WMAIN)
    include(Catch)
endif()

project_options(
    PREFIX "nese"
    ENABLE_CACHE
    # ${ENABLE_CPPCHECK}
    # ${ENABLE_CLANG_TIDY}
    ENABLE_VS_ANALYSIS
    # ENABLE_INTERPROCEDURAL_OPTIMIZATION
    # ENABLE_NATIVE_OPTIMIZATION
    # ${ENABLE_COVERAGE}
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

add_subdirectory("lib")
add_subdirectory("app")

if(FEATURE_TESTS)
    add_subdirectory("tests")
endif()

# Package the project
package_project(
    TARGETS
    nesesan.opengl
    nesesan
    nese
    nese_project_options 
    nese_project_warnings
)

# set the startup project for the "play" button in MSVC
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT nesesan.sdl2)

# Experience shows that explicit package naming can help make it easier to sort
# out potential ABI related issues before they start, while helping you
# track a build to a specific GIT SHA
set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

include(CPack)
