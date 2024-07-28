# Create the library
add_library(json STATIC
    json.cpp
    json_array.cpp
    json_formatter.cpp
    json_literal.cpp
    json_number.cpp
    json_object.cpp
    json_parser.cpp
    json_string.cpp)
add_library(Terra::JSON ALIAS json)

# Make project include directory available to external projects
target_include_directories(json
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

# Specify the C++ standard to observe
set_target_properties(json
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Use the following compile options
target_compile_options(json
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: >)

# Install target and associated include files
if(json_INSTALL)
    include(GNUInstallDirs)
    install(TARGETS json EXPORT jsonTargets ARCHIVE)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ TYPE INCLUDE)
    install(EXPORT jsonTargets
            FILE jsonConfig.cmake
            NAMESPACE Terra::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/json)
endif()
