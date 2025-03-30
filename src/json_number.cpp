/*
 *  json_number.cpp
 *
 *  Copyright (C) 2024-2025
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
#ifdef TERRA_DISABLE_STD_FORMAT
#include <iomanip>
#else
#include <format>
#endif
#include <cmath>
#include <terra/json/json.h>

namespace Terra::JSON
{

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
std::ostream &operator<<(std::ostream &o, const JSONNumber &value)
{
    // Floating point numbers need special treatment, so determine the type
    if (value.IsFloat())
    {
        JSONFloat number = value.GetFloat();

        // Infinity is not permitted on JSON
        if (std::isinf(number))
        {
            throw JSONException("Value of infinity is disallowed in JSON");
        }

        // NaN is not permitted on JSON
        if (std::isnan(number))
        {
            throw JSONException("Value of NaN is disallowed in JSON");
        }

        // If this is a -0 value, produce a 0
        if (number == -0.0) number = 0.0;

#ifndef TERRA_DISABLE_STD_FORMAT
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

/*
*  GetFloat()
*
*  Description:
*      Always return the number as a floating point value, even if it is an
*      integer.
*
*  Parameters:
*      None.
*
*  Returns:
*      The stored value as a double floating point value.
*
*  Comments:
*      None.
*/
JSONFloat JSONNumber::GetFloat() const
{
    // If it is a double, return it
    if (IsFloat()) return std::get<JSONFloat>(value);

    // Return the integer as a double
    return static_cast<JSONFloat>(std::get<JSONInteger>(value));
}

/*
*  GetInteger()
*
*  Description:
*      Always return the number as an integer value, even if it is a floating
*      point value.
*
*  Parameters:
*      None.
*
*  Returns:
*      The stored value as an integer value.
*
*  Comments:
*      None.
*/
JSONInteger JSONNumber::GetInteger() const
{
    // If it is a double, return it
    if (IsFloat()) return static_cast<JSONInteger>(std::get<double>(value));

    // Return the integer as a double
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
