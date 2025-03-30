# Change Log

v1.0.5

- If std::format is disabled, the <format> header should not be included

v1.0.4

- Removed recursive calls in the JSONParser and JSONPrinter, replacing recursion
  with a vector that maintains processing state
- Added operator==() and operator!=() functions for all JSON data types

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
