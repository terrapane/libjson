# Create the test excutable
add_executable(test_json test_json.cpp)

# Link to the required libraries
target_link_libraries(test_json Terra::JSON Terra::JSON Terra::stf)

# Specify the C++ standard to observe
set_target_properties(test_json
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(test_json
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: >)

# Ensure CTest can find the test
add_test(NAME test_json
         COMMAND test_json)
