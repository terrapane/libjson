/*
 *  json.h
 *
 *  Copyright (C) 2024-2025
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
 *      Each of the basic types above (except the JSON object) is defined
 *      as a struct that provides a member called "value" that is intended
 *      to facilitate direct access.  For example, JSONArray.object has a
 *      vector member named value over which one may iterate.
 *
 *      JSONArray and JSONObject each holds any number of JSON objects (not to
 *      be confused with JSONObject).
 *
 *      JSON was initially documented here: https://www.json.org/.  It is
 *      also formally defined in RFC 8259.
 *
 *      There is also a JSONParser that will parse and deserializes JSON text
 *      and forms a JSON object.  This object is not intended to be used by
 *      several threads at once, but this is a relatively light-weight object
 *      that can be instantiated, Parse() called, and destroyed as needed.
 *      This could be a simple function, but encapsulating the state within
 *      the object makes the interface cleaner.
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
 *      exception will be thrown.  Likewise, there are other functions that will
 *      throw a JSONException if a function call is invalid.  One must
 *      wrap all calls to these various objects in a try/catch block.
 *
 *      It is possible to output the JSON object by calling the ToString()
 *      function or using streaming operator like this:
 *
 *          oss << json_object;
 *
 *      The output of the ToString() or streaming operator produces a single
 *      line of output that is not formatted with any vertical whitespace or
 *      indentation.  To produce formatted output text, one may use the
 *      JSONFormatter.  The JSONFormatter accepts a JSON text string and
 *      reformats it using the specified amount of indentation.  It will
 *      essentially parse the text, but lacks some of the more rigid
 *      parsing logic in the JSONParser.  (In short, do not rely on the
 *      JSONFormatter to enforce proper syntax, but nonetheless do expect an
 *      exception if a syntax error is detected.)
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <ostream>
#include <cstdint>
#include <cstddef>
#include <variant>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <type_traits>
#include <limits>
#include <initializer_list>
#include <utility>

namespace Terra::JSON
{

// Define an exception to throw when parsing fails
class JSONException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// Make forward declarations
class JSON;
class JSONParser;

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
struct JSONString
{
    std::u8string value;

    JSONString() = default;
    JSONString(const std::u8string &string) : value{string} {}
    JSONString(std::u8string &&string) : value{std::move(string)} {}
    JSONString(const std::string &string) :
        value{std::u8string(string.cbegin(), string.cend())}
    {
    }
    JSONString(std::string &&string) :
        value{std::u8string(string.cbegin(), string.cend())}
    {
        // Performs a string copy, as there is no means to do move given
        // the data types are different
    }
    JSONString(const char8_t *string) : value{std::u8string(string)} {}
    JSONString(const char *string) :
        value{std::u8string(reinterpret_cast<const char8_t *>(string))}
    {
    }

    JSONString &operator=(const std::string &string)
    {
        value = std::u8string(string.cbegin(), string.cend());
        return *this;
    }
    JSONString &operator=(const std::u8string_view string)
    {
        value = std::u8string(string);
        return *this;
    }

    // Return the underlying string
    std::u8string &operator*() { return value; }
    const std::u8string &operator*() const { return value; }

    std::size_t Size() { return value.size(); }

    std::string ToString() const;
};

// Streaming operator for JSONString output
std::ostream &operator<<(std::ostream &o, const JSONString &string);

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

// Streaming operator for JSONLiteral output
std::ostream &operator<<(std::ostream &o, const JSONLiteral literal);

// JSON type to hold a JSON value type of number
struct JSONNumber
{
    std::variant<JSONInteger, JSONFloat> value;

    JSONNumber() = default;
    template<typename T,
             typename std::enable_if<std::is_floating_point<T>::value ||
                                         (std::is_integral<T>::value &&
                                          std::is_signed<T>::value),
                                     bool>::type = true>
    JSONNumber(T number) : value{number}
    {
    }
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value &&
                                         std::is_unsigned<T>::value,
                                     bool>::type = true>
    JSONNumber(T number)
    {
        if (number > std::numeric_limits<JSONInteger>::max())
        {
            throw JSONException("Unsigned integer exceeds limits");
        }
        value = static_cast<JSONInteger>(number);
    }
    ~JSONNumber() = default;

    bool IsFloat() const { return std::holds_alternative<JSONFloat>(value); }
    bool IsInteger() const { return !IsFloat(); }

    // Return the variant containing the number
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
};

// Streaming operator for JSONNumber output
std::ostream &operator<<(std::ostream &o, const JSONNumber &value);

// JSON type to hold a JSON value type of object
struct JSONObject
{
    std::map<std::u8string, JSON> value;

    JSONObject() = default;
    JSONObject(
        const std::initializer_list<std::pair<const std::u8string, JSON>>
                                                                        &list);
    JSONObject(
        const std::initializer_list<std::pair<const std::string, JSON>> &list);
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

    std::size_t Size() const { return value.size(); }

    std::string ToString() const;
};

// Streaming operator for JSONObject output
std::ostream &operator<<(std::ostream &o, const JSONObject &object);

// JSON type to hold a JSON value type of array
struct JSONArray
{
    std::vector<JSON> value;

    JSONArray() = default;
    JSONArray(const std::initializer_list<JSON> &list);
    ~JSONArray() = default;

    JSON &operator[](const std::size_t index);
    const JSON &operator[](const std::size_t index) const;

    // Return the underlying array of JSON objects
    std::vector<JSON> &operator*() { return value; }
    const std::vector<JSON> &operator*() const { return value; }

    std::size_t Size() const;

    std::string ToString() const;
};

// Streaming operator for JSONArray output
std::ostream &operator<<(std::ostream &o, const JSONArray &array);

// Define a type that will hold any one of the JSON types
using JSONValue =
    std::variant<JSONString, JSONNumber, JSONObject, JSONArray, JSONLiteral>;

// The JSON class holds a JSONValue
class JSON
{
    public:
        JSON() { AssignType(JSONValueType::Object); }
        JSON(JSONValueType type) { AssignType(type); }
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
        template<typename T,
                 typename std::enable_if<std::is_integral<T>::value ||
                                             std::is_floating_point<T>::value,
                                         bool>::type = true>
        JSON(T value) : value{JSONNumber(value)} {}

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

        template<typename T,
                 typename std::enable_if<std::is_floating_point<T>::value ||
                                             (std::is_integral<T>::value &&
                                              std::is_signed<T>::value),
                                         bool>::type = true>
        JSON &operator=(const T assignment)
        {
            value = JSONNumber(assignment);
            return *this;
        }
        template<typename T,
                 typename std::enable_if<std::is_integral<T>::value &&
                                             std::is_unsigned<T>::value,
                                         bool>::type = true>
        JSON &operator=(const T assignment)
        {
            value = JSONNumber(assignment);
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
            value = assignment;
            return *this;
        }

        JSON &operator=(const JSONString &assignment)
        {
            value = assignment;
            return *this;
        }
        JSON &operator=(JSONString &&assignment)
        {
            value = assignment;
            return *this;
        }

        JSON &operator=(const JSONArray &assignment)
        {
            value = assignment;
            return *this;
        }
        JSON &operator=(JSONArray &&assignment)
        {
            value = assignment;
            return *this;
        }

        JSON &operator=(const JSONObject &assignment)
        {
            value = assignment;
            return *this;
        }
        JSON &operator=(JSONObject &&assignment)
        {
            value = assignment;
            return *this;
        }

        // Functions to return a reference to the underlying JSONValue variant
        JSONValue &operator*() { return value; }
        const JSONValue &operator*() const { return value; }
        JSONValue &GetValue() { return value; }
        const JSONValue &GetValue() const { return value; }

        // Function to return a reference to the underlying type
        template<typename T>
        T &GetValue()
        {
            if (std::holds_alternative<T>(value))
            {
                return std::get<T>(value);
            }

            throw JSONException("JSON object contains a different value type");
        }
        template<typename T>
        const T &GetValue() const
        {
            if (std::holds_alternative<T>(value))
            {
                return std::get<T>(value);
            }

            throw JSONException("JSON object contains a different value type");
        }

        // Operators to ease access to the JSON information
        JSON &operator[](std::size_t index);
        const JSON &operator[](std::size_t index) const;
        JSON &operator[](const std::u8string &key);
        const JSON &operator[](const std::u8string &key) const;
        JSON &operator[](const std::string &key)
        {
            return operator[](std::u8string(key.cbegin(), key.cend()));
        }
        const JSON &operator[](const std::string &key) const
        {
            return operator[](std::u8string(key.cbegin(), key.cend()));
        }

        std::string ToString() const;

    protected:
        JSONValue value;
};

// Streaming operator for JSON output
std::ostream &operator<<(std::ostream &o, const JSON &json);

// Define the JSONParser object used to deserialize JSON text
class JSONParser
{
    public:
        JSONParser() = default;
        ~JSONParser() = default;

        JSON Parse(const std::string_view content);
        JSON Parse(const std::u8string_view content);

    protected:
        struct CompositeContext
        {
            JSONValue *value;
            bool opening_seen;
            bool member_seen;
            bool closing_seen;
        };

        constexpr bool EndOfInput() const { return p >= q; }
        constexpr std::size_t RemainingInput() const { return q - p; }
        constexpr void AdvanceReadPosition(std::size_t steps = 1)
        {
            auto old_p = p;
            p = std::min(q, p + steps);
            column += p - old_p;
        }
        void ConsumeWhitespace();
        JSONValueType DetermineValueType() const;
        JSONValue ParseInitialValue();
        JSONValue ParsePrimitiveValue(JSONValueType value_type);
        void ParseCompositeValue();
        JSONString ParseString();
        void ParseUnicode(JSONString &json_string);
        JSONNumber ParseNumber();
        void ParseObject();
        void ParseArray();
        JSONLiteral ParseLiteral();

        const char8_t *p;                       // Start of content
        const char8_t *q;                       // One past end of data
        std::size_t line;                       // Current line number
        std::size_t column;                     // Current column

        // Used to parse composite types
        std::vector<CompositeContext> composite_context;
};

// Define the JSONFormatter object used format JSON text
class JSONFormatter
{
    public:
        JSONFormatter(std::size_t indention = 2, bool allman_style = false) :
            o{nullptr},
            indention{indention},
            current_indention{0},
            allman_style{allman_style}
        {
        }
        ~JSONFormatter() = default;

        std::string Print(const JSON &json);
        std::string Print(const std::string_view content);
        std::string Print(const std::u8string_view content);
        void Print(std::ostream &o, const std::string_view content);
        void Print(std::ostream &o, const std::u8string_view content);

    protected:
        struct CompositeContext
        {
            JSONValue *value;
            bool opening_seen;
            bool member_seen;
            bool closing_seen;
        };

        constexpr bool EndOfInput() const { return p >= q; }
        constexpr std::size_t RemainingInput() const { return q - p; }
        constexpr void AdvanceReadPosition(std::size_t steps = 1)
        {
            auto old_p = p;
            p = std::min(q, p + steps);
            column += p - old_p;
        }
        void ProduceIndentation();
        void ConsumeWhitespace();
        JSONValueType DetermineValueType() const;

        void PrintInitialValue();
        void PrintPrimitiveValue(JSONValueType value_type);
        void PrintCompositeValue();

        void PrintString();
        void PrintNumber();
        void PrintObject();
        void PrintArray();
        void PrintLiteral();

        std::ostream *o;                        // Output stream pointer
        std::size_t indention;                  // Indention amount
        std::size_t current_indention;          // Current indention amount
        bool allman_style;                      // Allman coding style
        const char8_t *p;                       // Start of content
        const char8_t *q;                       // One past end of data
        std::size_t line;                       // Current line number
        std::size_t column;                     // Current column

        // Used to print composite types
        std::vector<CompositeContext> composite_context;
};

} // namespace Terra::JSON
