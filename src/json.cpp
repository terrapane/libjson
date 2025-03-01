/*
 *  json.cpp
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file contains implementation of some functions defined in the JSON
 *      object.
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
 *      Streaming operator to produce JSON text for a JSON type.
 *
 *  Parameters:
 *      o [in]
 *          A reference to the steaming operator onto which the JSON string
 *          will be appended.
 *
 *      json [in]
 *          The JSON object to output as JSON text.
 *
 *  Returns:
 *      A reference to the streaming operator passed in as input.
 *
 *  Comments:
 *      None.
 */
std::ostream &operator<<(std::ostream &o, const JSON &json)
{
    // Produce the object based on the type
    switch (json.GetValueType())
    {
        case JSONValueType::String:
            o << json.GetValue<JSONString>();
            break;

        case JSONValueType::Number:
            o << json.GetValue<JSONNumber>();
            break;

        case JSONValueType::Object:
            o << json.GetValue<JSONObject>();
            break;

        case JSONValueType::Array:
            o << json.GetValue<JSONArray>();
            break;

        case JSONValueType::Literal:
            o << json.GetValue<JSONLiteral>();
            break;

        default:
            throw JSONException("Unknown JSON object type");
    }

    return o;
}

/*
 *  JSON::GetValueType()
 *
 *  Description:
 *      Returns the type of JSON data held by this JSON object, which may
 *      be an object, string, number, or literal.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
JSONValueType JSON::GetValueType() const
{
    if (std::holds_alternative<JSONString>(value))
    {
        return JSONValueType::String;
    }
    if (std::holds_alternative<JSONNumber>(value))
    {
        return JSONValueType::Number;
    }
    if (std::holds_alternative<JSONObject>(value))
    {
        return JSONValueType::Object;
    }
    if (std::holds_alternative<JSONArray>(value))
    {
        return JSONValueType::Array;
    }
    if (std::holds_alternative<JSONLiteral>(value))
    {
        return JSONValueType::Literal;
    }

    throw JSONException("JSON object has no value");
}

/*
 *  JSON::AssignType()
 *
 *  Description:
 *      Assign the given JSON value type to this JSON object, replacing any
 *      previously assigned type.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void JSON::AssignType(JSONValueType type)
{
    switch(type)
    {
        case JSONValueType::String:
            value = JSONString();
            break;

        case JSONValueType::Number:
            value = JSONNumber();
            break;

        case JSONValueType::Object:
            value = JSONObject();
            break;

        case JSONValueType::Array:
            value = JSONArray();
            break;

        case JSONValueType::Literal:
            value = JSONLiteral();
            break;

        default:
            throw JSONException("Assignment of an unknown JSON value type");
            break;
    }
}

/*
 *  JSON::operator[]()
 *
 *  Description:
 *      This function is used to access the JSON object with the assumption
 *      that it is holding an array.  It will verify that it does hold an array
 *      and, if so, access the indexing operator of the array to retrieve
 *      the JSON object at that index.
 *
 *  Parameters:
 *      index [in]
 *          The index into the array.
 *
 *  Returns:
 *      A reference to the JSON object at the given index position of the array.
 *
 *  Comments:
 *      None.
 */
JSON &JSON::operator[](std::size_t index)
{
    // Ensure the JSON object holds an array
    if (!std::holds_alternative<JSONArray>(value))
    {
        throw JSONException("JSON object does not contain an array");
    }

    return std::get<JSONArray>(value)[index];
}

/*
 *  JSON::operator[]()
 *
 *  Description:
 *      This function is used to access the JSON object with the assumption
 *      that it is holding an array.  It will verify that it does hold an array
 *      and, if so, access the indexing operator of the array to retrieve
 *      the JSON object at that index.
 *
 *  Parameters:
 *      index [in]
 *          The index into the array.
 *
 *  Returns:
 *      A reference to the JSON object at the given index position of the array.
 *
 *  Comments:
 *      None.
 */
const JSON &JSON::operator[](std::size_t index) const
{
    // Ensure the JSON object holds an array
    if (!std::holds_alternative<JSONArray>(value))
    {
        throw JSONException("JSON object does not contain an array");
    }

    return std::get<JSONArray>(value)[index];
}

/*
 *  JSON::operator[]()
 *
 *  Description:
 *      This function is used to access the JSON object with the assumption
 *      that it is holding an object.  It will verify that it does hold an
 *      object and, if so, access the index operator of the object to
 *      retrieve the JSON object having the given key.
 *
 *  Parameters:
 *      key [in]
 *          The key used to access information in the JSON object.
 *
 *  Returns:
 *      A reference to the JSON object at the given index position of the array.
 *
 *  Comments:
 *      None.
 */
JSON &JSON::operator[](const std::u8string &key)
{
    // Ensure the JSON object holds an array
    if (!std::holds_alternative<JSONObject>(value))
    {
        throw JSONException("JSON object does not contain an object type");
    }

    return std::get<JSONObject>(value)[key];
}

/*
 *  JSON::operator[]()
 *
 *  Description:
 *      This function is used to access the JSON object with the assumption
 *      that it is holding an object.  It will verify that it does hold an
 *      object and, if so, access the index operator of the object to
 *      retrieve the JSON object having the given key.
 *
 *  Parameters:
 *      key [in]
 *          The key used to access information in the JSON object.
 *
 *  Returns:
 *      A reference to the JSON object at the given index position of the array.
 *
 *  Comments:
 *      None.
 */
const JSON &JSON::operator[](const std::u8string &key) const
{
    // Ensure the JSON object holds an array
    if (!std::holds_alternative<JSONObject>(value))
    {
        throw JSONException("JSON object does not contain an object type");
    }

    return std::get<JSONObject>(value)[key];
}

/*
 *  JSON::ToString()
 *
 *  Description:
 *      This function will convert the JSON object to a std::string.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A std::string containing JSON text derived from the JSON object.
 *
 *  Comments:
 *      None.
 */
std::string JSON::ToString() const
{
    std::ostringstream oss;

    oss << *this;

    return oss.str();
}

} // namespace Terra::JSON
