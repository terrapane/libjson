/*
 *  json.cpp
 *
 *  Copyright (C) 2024, 2025
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
 *  JSON::JSON()
 *
 *  Description:
 *      Default constructor for the JSON object.  It will assign a type of
 *      JSONValueType::Object to the new object.
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
JSON::JSON() { AssignType(JSONValueType::Object); }

/*
 *  JSON::JSON()
 *
 *  Description:
 *      Constructor for the JSON object used to initialize the object to
 *      contain the specified object type.
 *
 *  Parameters:
 *      type [in]
 *          The type of data the JSON object is to hold.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
JSON::JSON(JSONValueType type) { AssignType(type); }

/*
 *  JSON::JSON()
 *
 *  Description:
 *      Constructor for the JSON object used to initialize the object with
 *      a JSONNumber type by specifying literal number values.
 *
 *  Parameters:
 *      number [in]
 *          The value to assign to the JSONNumber stored inside the object.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
template<typename T>
    requires std::floating_point<T> || std::integral<T>
JSON::JSON(T number) : value{JSONNumber(number)}
{
}

//Specializations for the different type of C++ integer / floating point values
template JSON::JSON(int);
template JSON::JSON(long);
template JSON::JSON(long long);
template JSON::JSON(unsigned int);
template JSON::JSON(unsigned long);
template JSON::JSON(unsigned long long);
template JSON::JSON(float);
template JSON::JSON(double);

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
 *  JSON::operator=()
 *
 *  Description:
 *      Assignment operator for the JSON object used to assign a numeric value
 *      that gets stored as a JSONNumber type.
 *
 *  Parameters:
 *      number [in]
 *          The value to assign to the JSONNumber stored inside the object.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
template<typename T>
    requires std::floating_point<T> || std::integral<T>
JSON &JSON::operator=(const T number)
{
    value = JSONNumber(number);
    return *this;
}

//Specializations for the different type of C++ integer / floating point values
template JSON &JSON::operator=(int);
template JSON &JSON::operator=(long);
template JSON &JSON::operator=(long long);
template JSON &JSON::operator=(unsigned int);
template JSON &JSON::operator=(unsigned long);
template JSON &JSON::operator=(unsigned long long);
template JSON &JSON::operator=(float);
template JSON &JSON::operator=(double);

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
 *      A reference to the value for the given key.
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
 *      A constant reference to the value for the given key.
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
 *      A reference to the value for the given key.
 *
 *  Comments:
 *      None.
 */
JSON &JSON::operator[](const std::string &key)
{
    return operator[](std::u8string(key.cbegin(), key.cend()));
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
 *      A constant reference to the value for the given key.
 *
 *  Comments:
 *      None.
 */
const JSON &JSON::operator[](const std::string &key) const
{
    return operator[](std::u8string(key.cbegin(), key.cend()));
}

/*
 *  operator==()
 *
 *  Description:
 *      This function will compare two JSON objects for equality.
 *
 *  Parameters:
 *      other [in]
 *          The other JSON object against which to compare.
 *
 *  Returns:
 *      True if this and the other object are equal, and false if they are not.
 *
 *  Comments:
 *      None.
 */
bool JSON::operator==(const JSON &other) const
{
    // If this and the other don't have the same type, they are not equal
    if (value.index() != other.value.index()) return false;

    // Given they have the same type, compare by types
    if (std::holds_alternative<JSONString>(value))
    {
        return std::get<JSONString>(value) == std::get<JSONString>(other.value);
    }
    if (std::holds_alternative<JSONNumber>(value))
    {
        return std::get<JSONNumber>(value) == std::get<JSONNumber>(other.value);
    }
    if (std::holds_alternative<JSONObject>(value))
    {
        return std::get<JSONObject>(value) == std::get<JSONObject>(other.value);
    }
    if (std::holds_alternative<JSONArray>(value))
    {
        return std::get<JSONArray>(value) == std::get<JSONArray>(other.value);
    }
    if (std::holds_alternative<JSONLiteral>(value))
    {
        return std::get<JSONLiteral>(value) ==
               std::get<JSONLiteral>(other.value);
    }

    // We should never get to this point
    throw JSONException("JSON object has an unexpected variant type");
}

/*
 *  operator!=()
 *
 *  Description:
 *      This function will compare two JSON objects for inequality.
 *
 *  Parameters:
 *      other [in]
 *          The other JSON object against which to compare.
 *
 *  Returns:
 *      True if this and the other object are not equal, and false if they are
 *      equal.
 *
 *  Comments:
 *      None.
 */
bool JSON::operator!=(const JSON &other) const
{
    return !operator==(other);
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
std::ostream &operator<<(std::ostream &o, const Terra::JSON::JSON &json)
{
    // Produce the object based on the type
    switch (json.GetValueType())
    {
        case Terra::JSON::JSONValueType::String:
            o << std::get<Terra::JSON::JSONString>(*json);
            break;

        case Terra::JSON::JSONValueType::Number:
            o << std::get<Terra::JSON::JSONNumber>(*json);
            break;

        case Terra::JSON::JSONValueType::Object:
            o << std::get<Terra::JSON::JSONObject>(*json);
            break;

        case Terra::JSON::JSONValueType::Array:
            o << std::get<Terra::JSON::JSONArray>(*json);
            break;

        case Terra::JSON::JSONValueType::Literal:
            o << std::get<Terra::JSON::JSONLiteral>(*json);
            break;

        default:
            throw Terra::JSON::JSONException("Unknown JSON object type");
    }

    return o;
}
