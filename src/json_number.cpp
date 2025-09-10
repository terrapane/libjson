/*
 *  json_number.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file contains implementation of some functions defined for the
 *      JSONNumber object.
 *
 *  Portability Issues:
 *      None.
 */

#include <sstream>
#include <cmath>
#include <terra/json/json.h>
#include "has_format.h"

namespace Terra::JSON
{

/*
 *  JSONNumber::operator==()
 *
 *  Description:
 *      Equality operator for JSONNumber.
 *
 *  Parameters:
 *      Other JSONNumber object against which to compare.
 *
 *  Returns:
 *      True if equal, false if not equal.
 *
 *  Comments:
 *      None.
 */
bool JSONNumber::operator==(const JSONNumber &other) const
{
    // If they're not the same type, then they are not equal
    if (value.index() != other.value.index()) return false;

    // Compare values assuming the same type
    return std::visit([](auto first, auto second) { return first == second; },
                      value,
                      other.value);
}

/*
 *  JSONNumber::operator!=()
 *
 *  Description:
 *      Inequality operator for JSONNumber.
 *
 *  Parameters:
 *      Other JSONNumber object against which to compare.
 *
 *  Returns:
 *      True if not equal, false if equal.
 *
 *  Comments:
 *      None.
 */
bool JSONNumber::operator!=(const JSONNumber &other) const
{
    return !operator==(other);
}

/*
 *  JSONNumber::GetFloat()
 *
 *  Description:
 *      Return the JSONNumber as a floating point value.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Returns the JSONNumber as a floating point value, casting from
 *      integer if the internal value is an integer.
 *
 *  Comments:
 *      None.
 */
JSONFloat JSONNumber::GetFloat() const
{
    // If holding a JSONFloat, return it
    if (IsFloat()) return std::get<JSONFloat>(value);

    // Return the JSONInteger value cast as a JSONFloat
    return static_cast<JSONFloat>(std::get<JSONInteger>(value));
}

/*
 *  JSONNumber::GetInteger()
 *
 *  Description:
 *      Return the JSONNumber as an integer value.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Returns the JSONNumber as an integer value, casting from floating point
 *      if the internal value is a floating point value.
 *
 *  Comments:
 *      None.
 */
JSONInteger JSONNumber::GetInteger() const
{
    // If holding a JSONFloat, return the value cast as a JSONInteger
    if (IsFloat())
    {
        return static_cast<JSONInteger>(std::get<JSONFloat>(value));
    }

    // Return the integer value
    return std::get<JSONInteger>(value);
}

/*
 *  JSONNumber::ToString()
 *
 *  Description:
 *      This function will convert the JSONNumber object to a std::string.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A std::string containing JSON text derived from the JSONNumber object.
 *
 *  Comments:
 *      None.
 */
std::string JSONNumber::ToString() const
{
    std::ostringstream oss;

    oss << *this;

    return oss.str();
}

} // namespace Terra::JSON

/*
 *  operator<<()
 *
 *  Description:
 *      Streaming operator to produce JSON text for a JSONNumber type.
 *
 *  Parameters:
 *      o [in]
 *          A reference to the steaming operator onto which the JSON string
 *          will be appended.
 *
 *      value [in]
 *          The numeric value to output as JSON text.
 *
 *  Returns:
 *      A reference to the streaming operator passed in as input.
 *
 *  Comments:
 *      None.
 */
std::ostream &operator<<(std::ostream &o, const Terra::JSON::JSONNumber &value)
{
    // Floating point numbers need special treatment, so determine the type
    if (value.IsFloat())
    {
        Terra::JSON::JSONFloat number = value.GetFloat();

        // Infinity is not permitted on JSON
        if (std::isinf(number))
        {
            throw Terra::JSON::JSONException(
                "Value of infinity is disallowed in JSON");
        }

        // NaN is not permitted on JSON
        if (std::isnan(number))
        {
            throw Terra::JSON::JSONException(
                "Value of NaN is disallowed in JSON");
        }

        // If this is a -0 value, produce a 0
        if (number == -0.0) number = 0.0;

#ifdef TERRA_HAS_FORMAT
        o << std::format("{}", number);
#else
        // This results in a lower-precision output vs. std::format
        o << number;
#endif
    }
    else
    {
        o << value.GetInteger();
    }

    return o;
}
