/*
 *  json_object.cpp
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
 *      JSONObject object.
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
 *      Streaming operator to produce JSON text for a JSONObject type.
 *
 *  Parameters:
 *      o [in]
 *          A reference to the steaming operator onto which the JSON string
 *          will be appended.
 *
 *      object [in]
 *          The JSONObject to output as JSON text.
 *
 *  Returns:
 *      A reference to the streaming operator passed in as input.
 *
 *  Comments:
 *      None.
 */
std::ostream &operator<<(std::ostream &o, const JSONObject &object)
{
    bool need_comma = false;

    // Output the opening brace
    o << '{';

    // Iterate over the map, outputting each element
    for (const auto &[key, value] : *object)
    {
        if (need_comma) o << ", ";
        o << JSONString(key) << ": " << value;
        need_comma = true;
    }

    // Output the closing brace
    o << '}';

    return o;
}

/*
 *  JSONObject::JSONObject()
 *
 *  Description:
 *      This is a constructor for the JSONObject object that will accept an
 *      initializer list to use in assignment.
 *
 *  Parameters:
 *      list [in]
 *          The initializer list to use for object initialization.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
JSONObject::JSONObject(
    const std::initializer_list<std::pair<const std::u8string, JSON>> &list) :
    value{list}
{
}

/*
 *  JSONObject::JSONObject()
 *
 *  Description:
 *      This is a constructor for the JSONObject object that will accept an
 *      initializer list to use in assignment.
 *
 *  Parameters:
 *      list [in]
 *          The initializer list to use for object initialization.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
JSONObject::JSONObject(
    const std::initializer_list<std::pair<const std::string, JSON>> &list) :
    value{}
{
    // Add items from the list into the map, converting the string type
    for (const auto &item : list)
    {
        std::u8string item_string(item.first.cbegin(), item.first.cend());
        value.insert({item_string, item.second});
    }
}

/*
 *  operator==()
 *
 *  Description:
 *      This function will compare two JSONObject objects for equality.
 *
 *  Parameters:
 *      other [in]
 *          The other JSONObject object against which to compare.
 *
 *  Returns:
 *      True if this and the other object are equal, and false if they are not.
 *
 *  Comments:
 *      None.
 */
bool JSONObject::operator==(const JSONObject &other) const
{
    return value == other.value;
}

/*
 *  operator!=()
 *
 *  Description:
 *      This function will compare two JSONObject objects for inequality.
 *
 *  Parameters:
 *      other [in]
 *          The other JSONObject object against which to compare.
 *
 *  Returns:
 *      True if this and the other object are not equal, and false if they are
 *      equal.
 *
 *  Comments:
 *      None.
 */
bool JSONObject::operator!=(const JSONObject &other) const
{
    return !operator==(other);
}

/*
 *  JSONObject::ToString()
 *
 *  Description:
 *      This function will convert the JSONObject object to a std::string.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A std::string containing JSON text derived from the JSONObject object.
 *
 *  Comments:
 *      None.
 */
std::string JSONObject::ToString() const
{
    std::ostringstream oss;

    oss << *this;

    return oss.str();
}

} // namespace Terra::JSON
