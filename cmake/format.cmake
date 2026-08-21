#
# Check to see if std::format is supported
#
# While __cpp_lib_format can indicate support, older Mac versions will
# prevent use since older OS releases do not support it.  This will attempt
# to build code that uses std::format to see if the target version supports it.
#

include(CheckFunctionExists)
include(CheckCXXSourceCompiles)

# Function to check for for std::format support
function(PerformStdFormatCheck)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Check to see if std::format is available
    check_cxx_source_compiles([[
        #include <format>
        #include <string>
        int main()
        {
            const std::string result = std::format("file {}, line {}",
                                                   __FILE__,
                                                   __LINE__);
            return result.empty() ? 1 : 0;
        }
    ]] _LOCAL_HAVE_FORMAT)

    # Export to the parent scope
    if(_LOCAL_HAVE_FORMAT)
        set(HAVE_STD_FORMAT TRUE PARENT_SCOPE)
    endif()
endfunction()

PerformStdFormatCheck()
