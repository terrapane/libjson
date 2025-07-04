cmake_minimum_required(VERSION 3.21)

# Define the JSON Library project
project(libjson
        VERSION 1.0.7.0
        DESCRIPTION "JSON Library"
        LANGUAGES CXX)

# Tests are built by default when this is a top-level project
if(PROJECT_IS_TOP_LEVEL)
    option(libjson_BUILD_TESTS "Build Tests for the JSON Library" ON)
else()
    option(libjson_BUILD_TESTS "Build Tests for the JSON Library" OFF)
endif()

# Option to control ability to install the library
option(libjson_INSTALL "Install the JSON Library" ON)

# Determine whether clang-tidy will be performed
option(libjson_CLANG_TIDY "Use clang-tidy to perform linting during build" OFF)

add_subdirectory(dependencies)
add_subdirectory(src)

include(CTest)

if(BUILD_TESTING AND libjson_BUILD_TESTS)
    add_subdirectory(test)
endif()
