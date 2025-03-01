/*
 *  json_array.cpp
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
 *      JSONArray object.
 *
 *  Portability Issues:
 *      None.
 */

#include <sstream>
#include <terra/json/json.h>

namespace Terra::JSON
{

/*
 *  operator<<()
 *
 *  Description:
 *      Streaming operator to produce JSON text for a JSONArray type.
 *
 *  Parameters:
 *      o [in]
 *          A reference to the steaming operator onto which the JSON string
 *          will be appended.
 *
 *      array [in]
 *          The JSONArray to output as JSON text.
 *
 *  Returns:
 *      A reference to the streaming operator passed in as input.
 *
 *  Comments:
 *      None.
 */
std::ostream &operator<<(std::ostream &o, const JSONArray &array)
{
    bool need_comma = false;

    // Output the opening array bracket
    o << '[';

    // Iterate over the array, outputting each element
    for (const auto &item : *array)
    {
        if (need_comma) o << ", ";
        o << item;
        need_comma = true;
    }

    // Output the closing array bracket
    o << ']';

    return o;
}

/*
 *  JSONArray::JSONArray()
 *
 *  Description:
 *      This is a constructor for the JSONArray object that will accept an
 *      array of JSON objects as initialization values.
 *
 *  Parameters:
 *      list [in]
 *          A list of JSON objects with which to initialize the array.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
JSONArray::JSONArray(const std::initializer_list<JSON> &list) : value{}
{
    value.insert(value.end(), list);
}

/*
 *  JSONArray::operator[]()
 *
 *  Description:
 *      Convenience operator to provide access to members of the array.
 *
 *  Parameters:
 *      index [in]
 *          The index into the array.
 *
 *  Returns:
 *      A reference to the JSON element in the array at the given index.
 *
 *  Comments:
 *      None.
 */
JSON &JSONArray::operator[](const std::size_t index)
{
    return value[index];
}

/*
 *  JSONArray::operator[]()
 *
 *  Description:
 *      Convenience operator to provide access to members of the array.
 *
 *  Parameters:
 *      index [in]
 *          The index into the array.
 *
 *  Returns:
 *      A reference to the JSON element in the array at the given index.
 *
 *  Comments:
 *      None.
 */
const JSON &JSONArray::operator[](const std::size_t index) const
{
    return value[index];
}

/*
 *  JSONArray::Size()
 *
 *  Description:
 *      Returns the number of elements contained in the JSON array.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      The number of JSON elements contained in the JSONArray (i.e., the
 *      size of the underlying value vector).
 *
 *  Comments:
 *      None.
 */
std::size_t JSONArray::Size() const
{
    return value.size();
}

/*
 *  JSONArray::ToString()
 *
 *  Description:
 *      This function will convert the JSONArray object to a std::string.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A std::string containing JSON text derived from the JSONArray object.
 *
 *  Comments:
 *      None.
 */
std::string JSONArray::ToString() const
{
    std::ostringstream oss;

    oss << *this;

    return oss.str();
}

} // namespace Terra::JSON
