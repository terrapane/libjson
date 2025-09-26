# Create the test executable
add_executable(test_json_parser test_json_parser.cpp)

# Link to the required libraries
target_link_libraries(test_json_parser Terra::json Terra::stf)

# Specify the C++ standard to observe
set_target_properties(test_json_parser
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(test_json_parser
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: >)

# Ensure CTest can find the test
add_test(NAME test_json_parser
         COMMAND test_json_parser)
