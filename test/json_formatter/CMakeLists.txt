# Create the test excutable
add_executable(test_json_formatter test_json_formatter.cpp)

# Link to the required libraries
target_link_libraries(test_json_formatter Terra::json Terra::stf)

# Specify the C++ standard to observe
set_target_properties(test_json_formatter
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(test_json_formatter
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: >)

# Ensure CTest can find the test
add_test(NAME test_json_formatter
         COMMAND test_json_formatter)
