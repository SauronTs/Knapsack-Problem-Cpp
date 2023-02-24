cmake_minimum_required(VERSION 3.14)
project(cpp_gen_alg)

# try to prevent in-source builds
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate 'build' directory and build in there.")
endif()

# set default c++ standard for compile targets
set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-Wall -Wextra -Wconversion -pedantic -Wfatal-errors)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory("src")
