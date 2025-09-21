# Change Log

v1.0.9

- Removed use of raw pointers
- Renamed ParseInitialValue and PrintInitialValue functions
- Updated dependencies

v1.0.8

- Worked to reduce binary size
- CMake changes to support downstream unit testing
- Updated dependencies

v1.0.7

- Changed the conditional compilation logic for use of std::format

v1.0.6

- Updated dependencies
- Made compiler warnings stricter
- Addressed warnings

v1.0.5

- If std::format is disabled, the \<format\> header should not be included
- Remove the constexpr on `ConvertToStdString()`, as that doesn't work on
  slightly older systems and it doesn't add significant benefit

v1.0.4

- Removed recursive calls in the JSONParser and JSONPrinter, replacing recursion
  with a vector that maintains processing state
- Added `operator==()` and `operator!=()` functions for all JSON data types

v1.0.3

- Made use of std::format configurable since some older macOS releases
  to not support it
- Updated to the latest Simple Test Framework (STF)

v1.0.2

- Added the ability to disable use of std::format for the benefit of building
  for legacy macOS systems, as systems older than macOS 13 do not support it

v1.0.1

- Added additional `operator*()` functions to make access to data consistent
- Added HasKey() to JSONObject types to ease determining if keys exist

v1.0.0

- Initial Release
