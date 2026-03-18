cmake_minimum_required(VERSION 3.17)
project(globals VERSION 1.0 LANGUAGES CXX)

# globals_configure(<target> [GLOBLIST_DIR <dir>])
#
# Integrates the globals library into an existing CMake target by:
#   - Adding globals.cc to the target's sources (so it compiles alongside your
#     code and picks up your project's globlist.h from your include paths).
#   - Adding the globals source directory to the target's include path (for globals.h).
#
# If your globlist.h is not already on the target's include path, pass its
# directory with GLOBLIST_DIR and it will be added (with higher priority).
#
# Example (globlist.h alongside CMakeLists.txt):
#   include(FetchContent)
#   FetchContent_Declare(globals GIT_REPOSITORY https://github.com/.../globals.git)
#   FetchContent_MakeAvailable(globals)
#   add_executable(my_program main.cc)
#   globals_configure(my_program)
#
# Example (globlist.h in a subdirectory):
#   globals_configure(my_program GLOBLIST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config)
function(globals_configure TARGET)
    cmake_parse_arguments(ARG "" "GLOBLIST_DIR" "" ${ARGN})
    target_sources(${TARGET} PRIVATE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/globals.cc")
    target_include_directories(${TARGET} PRIVATE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")
    if(ARG_GLOBLIST_DIR)
        target_include_directories(${TARGET} BEFORE PRIVATE "${ARG_GLOBLIST_DIR}")
    endif()
endfunction()
