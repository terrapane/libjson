/*
 *  json.h
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the JSON object and structures/objects that represent
 *      each of the JSON types.  Those types are:
 *          JSONLiteral - Represents a JSON literal type (e.g., null)
 *          JSONNumber - An integer or floating point value
 *          JSONString - A JSON string (this MUST be in UTF-8!)
 *          JSONArray - An array of JSON types
 *          JSONObject - A map of string/JSON type pairs
 *
 *      The various types are represented via a type called JSONValue, which is
 *      used by the object called JSON.  The JSON object provides various
 *      functions to make it easy to access the various types and is the type
 *      of object produced by the JSONParser.
 *
 *      JSONArray and JSONObject each holds any number of JSON objects (not to
 *      be confused with JSONObject).
 *
 *      JSON was initially documented here: https://www.json.org/.  It is also
 *      formally defined in RFC 8259.
 *
 *      There is also a JSONParser that will parse and deserializes JSON text
 *      and forms a JSON object (see json_parser.h).
 *
 *      It is possible to create JSON objects by using initializer lists,
 *      assignment, etc. The various test functions provide example usage.
 *
 *      The strings used by these objects are std::u8string, though there are
 *      functions that make it easy to provide std::string as input.  However,
 *      it is important that all strings are valid UTF-8, as this is required
 *      by the JSON specification.
 *
 *      If there is an error parsing JSON text or producing JSON text, an
 *      JSONException exception will be thrown.  Likewise, there are other
 *      functions that will throw a JSONException for various reasons (e.g.,
 *      if the UTF-8 string not properly formatted, an exception will occur
 *      trying to create formatted output).  One must wrap all calls to these
 *      various objects in a try/catch block.
 *
 *      It is possible to output the JSON object by calling the ToString()
 *      function or using streaming operator like this:
 *
 *          oss << json_object;
 *
 *      The output of the ToString() or streaming operator produces a single
 *      line of output that is not formatted with any vertical whitespace or
 *      indentation.  To produce formatted output text, one may use the
 *      JSONFormatter (see json_formatter.h).
 *
 *  Portability Issues:
 *      Requires C++20 or later.
 */

#pragma once

#include <ostream>
#include <cstdint>
#include <cstddef>
#include <variant>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <type_traits>
#include <limits>
#include <initializer_list>
#include <utility>
#include <concepts>
#include "json_exception.h"

namespace Terra::JSON
{

// Make forward declarations
class JSON;

// Define an enumeration for the JSON value types
enum class JSONValueType
{
    String,
    Number,
    Object,
    Array,
    Literal
};

// JSON type to hold a JSON value type of string
class JSONString
{
    public:
        JSONString() = default;
        JSONString(const std::u8string_view string) : value{string} {}
        JSONString(std::u8string &&string) : value{std::move(string)} {}
        JSONString(const std::string_view string);
        JSONString(std::string &&string);
        JSONString(const char8_t *string) : value{string} {}
        JSONString(const char *string) :
            value{std::u8string(reinterpret_cast<const char8_t *>(string))}
        {
        }

        JSONString &operator=(const std::string_view string);
        JSONString &operator=(const std::u8string_view string)
        {
            value = string;
            return *this;
        }

        bool operator==(const JSONString &other) const
        {
            return value == other.value;
        }
        bool operator!=(const JSONString &other) const
        {
            return !operator==(other);
        }

        // Return the underlying string
        std::u8string &operator*() { return value; }
        const std::u8string &operator*() const { return value; }

        std::size_t Size() const noexcept { return value.size(); }

        std::string ToString() const;

    protected:
        std::u8string value;
};

// Define JSON Number types for convenience; these should be the largest
// possible numeric types (one integer and one floating point)
using JSONInteger = long long;
using JSONFloat = double;

// JSON type to hold a JSON value type of literal
enum class JSONLiteral
{
    True,
    False,
    Null
};

// JSON type to hold a JSON value type of number
class JSONNumber
{
    public:
        JSONNumber() = default;
        template<typename T>
            requires std::floating_point<T> || std::integral<T>
        JSONNumber(T number)
        {
            if constexpr (std::floating_point<T>)
            {
                value = static_cast<JSONFloat>(number);
            }
            else if constexpr (std::unsigned_integral<T>)
            {
                // Insure the unsigned value does not exceed the max value of a
                // signed integer, as we only store a signed integer type
                if (number > std::numeric_limits<JSONInteger>::max())
                {
                    throw JSONException("Unsigned integer exceeds limits");
                }
                value = static_cast<JSONInteger>(number);
            }
            else
            {
                value = static_cast<JSONInteger>(number);
            }
        }
        ~JSONNumber() = default;

        bool operator==(const JSONNumber &other) const;
        bool operator!=(const JSONNumber &other) const;

        bool IsFloat() const
        {
            return std::holds_alternative<JSONFloat>(value);
        }
        bool IsInteger() const { return !IsFloat(); }

        // Return a reference to the variant containing the number
        std::variant<JSONInteger, JSONFloat> &operator*() { return value; }
        const std::variant<JSONInteger, JSONFloat> &operator*() const
        {
            return value;
        }

        // One may call these regardless of the held value and the number
        // will be cast to the requested type as required
        JSONFloat GetFloat() const;
        JSONInteger GetInteger() const;

        std::string ToString() const;

    protected:
        std::variant<JSONInteger, JSONFloat> value;
};

// JSON type to hold a JSON value type of object
class JSONObject
{
    public:
        JSONObject() = default;
        JSONObject(
            const std::initializer_list<std::pair<const std::u8string, JSON>>
                &list);
        JSONObject(
            const std::initializer_list<std::pair<const std::string, JSON>>
                &list);
        ~JSONObject() = default;

        JSON &operator[](const std::u8string &key) { return value[key]; }
        const JSON &operator[](const std::u8string &key) const
        {
            return value.at(key);
        }
        JSON &operator[](const std::string &key)
        {
            return operator[](std::u8string(key.cbegin(), key.cend()));
        }
        const JSON &operator[](const std::string &key) const
        {
            return operator[](std::u8string(key.cbegin(), key.cend()));
        }

        bool HasKey(const std::u8string &key) const
        {
            return (value.count(key) > 0);
        }
        bool HasKey(const std::string &key) const
        {
            return HasKey(std::u8string(key.cbegin(), key.cend()));
        }

        // Return the underlying map of JSON objects
        std::map<std::u8string, JSON> &operator*() { return value; }
        const std::map<std::u8string, JSON> &operator*() const { return value; }

        std::size_t Size() const noexcept { return value.size(); }

        bool operator==(const JSONObject &other) const;
        bool operator!=(const JSONObject &other) const;

        std::string ToString() const;

    protected:
        std::map<std::u8string, JSON> value;
};

// JSON type to hold a JSON value type of array
class JSONArray
{
    public:
        JSONArray() = default;
        JSONArray(const std::initializer_list<JSON> &list);
        ~JSONArray() = default;

        JSON &operator[](const std::size_t index);
        const JSON &operator[](const std::size_t index) const;

        // Return the underlying array of JSON objects
        std::vector<JSON> &operator*() { return value; }
        const std::vector<JSON> &operator*() const { return value; }

        bool operator==(const JSONArray &other) const;
        bool operator!=(const JSONArray &other) const;

        std::size_t Size() const noexcept;

        std::string ToString() const;

    protected:
        std::vector<JSON> value;
};

// Define a type that will hold any one of the JSON types
using JSONValue =
    std::variant<JSONString, JSONNumber, JSONObject, JSONArray, JSONLiteral>;

// The JSON class holds a JSONValue
class JSON
{
    public:
        JSON();
        JSON(JSONValueType type);
        JSON(const JSONValue &value) : value{value} {}
        JSON(JSONValue &&value) : value{std::move(value)} {}
        JSON(const JSONString &string) : value{string} {}
        JSON(JSONString &&string) : value{std::move(string)} {}
        JSON(const JSONNumber &number) : value{number} {}
        JSON(JSONNumber &&number) : value{std::move(number)} {}
        JSON(const JSONObject &object) : value{object} {}
        JSON(JSONObject &&object) : value{std::move(object)} {}
        JSON(const JSONArray &array) : value{array} {}
        JSON(JSONArray &&array) : value{std::move(array)} {}
        JSON(const JSONLiteral value) : value{value} {}
        JSON(const char8_t *string) : JSON(JSONString(string)) {}
        JSON(const char *string) :
            JSON(JSONString(reinterpret_cast<const char8_t *>(string)))
        {
        }
        template<typename T>
            requires std::floating_point<T> || std::integral<T>
        JSON(T number) : value{JSONNumber(number)}
        {
        }

        ~JSON() = default;

        // Return the type of the JSON value held by this object
        JSONValueType GetValueType() const;

        // Assign the given JSON value type to this JSON object, replacing any
        // previously assigned type
        void AssignType(JSONValueType type);

        // Note that all of the following assignment operators assign values
        // and do not parse JSON text

        JSON &operator=(const char8_t *string)
        {
            value = JSONString(string);
            return *this;
        }
        JSON &operator=(const char *string)
        {
            value = JSONString(string);
            return *this;
        }

        template<typename T>
            requires std::floating_point<T> || std::integral<T>
        JSON &operator=(const T number)
        {
            value = JSONNumber(number);
            return *this;
        }

        JSON &operator=(const JSONLiteral assignment)
        {
            value = assignment;
            return *this;
        }

        JSON &operator=(const JSONNumber &assignment)
        {
            value = assignment;
            return *this;
        }
        JSON &operator=(JSONNumber &&assignment)
        {
            value = std::move(assignment);
            return *this;
        }

        JSON &operator=(const JSONString &assignment)
        {
            value = std::move(assignment);
            return *this;
        }
        JSON &operator=(JSONString &&assignment)
        {
            value = std::move(assignment);
            return *this;
        }

        JSON &operator=(const JSONArray &assignment)
        {
            value = assignment;
            return *this;
        }
        JSON &operator=(JSONArray &&assignment)
        {
            value = std::move(assignment);
            return *this;
        }

        JSON &operator=(const JSONObject &assignment)
        {
            value = assignment;
            return *this;
        }
        JSON &operator=(JSONObject &&assignment)
        {
            value = std::move(assignment);
            return *this;
        }

        // Functions to return a reference to the underlying JSONValue variant
        JSONValue &operator*() { return value; }
        const JSONValue &operator*() const { return value; }

        // Operators to ease access to the JSON information
        JSON &operator[](std::size_t index);
        const JSON &operator[](std::size_t index) const;
        JSON &operator[](const std::u8string &key);
        const JSON &operator[](const std::u8string &key) const;
        JSON &operator[](const std::string &key);
        const JSON &operator[](const std::string &key) const;

        bool operator==(const JSON &other) const;
        bool operator!=(const JSON &other) const;

        std::string ToString() const;

    protected:
        JSONValue value;
};

} // namespace Terra::JSON

// Streaming operator for JSONString output
std::ostream &operator<<(std::ostream &o,
                         const Terra::JSON::JSONString &string);

// Streaming operator for JSONLiteral output
std::ostream &operator<<(std::ostream &o,
                         const Terra::JSON::JSONLiteral literal);

// Streaming operator for JSONNumber output
std::ostream &operator<<(std::ostream &o, const Terra::JSON::JSONNumber &value);

// Streaming operator for JSONObject output
std::ostream &operator<<(std::ostream &o,
                         const Terra::JSON::JSONObject &object);

// Streaming operator for JSONArray output
std::ostream &operator<<(std::ostream &o, const Terra::JSON::JSONArray &array);

// Streaming operator for JSON output
std::ostream &operator<<(std::ostream &o, const Terra::JSON::JSON &json);
