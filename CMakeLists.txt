cmake_minimum_required(VERSION 3.16)
project(cybershell)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 17)
IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
ENDIF()

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_subdirectory(src)
