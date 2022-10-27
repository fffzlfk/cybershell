cmake_minimum_required(VERSION 3.23)
project(cybershell)

set(CMAKE_EXPORT_COMPILECOMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_subdirectory(src)