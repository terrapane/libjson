/*
 *  json_parser.cpp
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the JSON parsing and serialization object.  Note
 *      that this parser does not utilize recursive calls, thus processing is
 *      only bound by the available system memory.
 *
 *  Portability Issues:
 *      None.
 */

#include <cctype>
#include <format>
#ifndef __cpp_lib_format
#include <sstream>
#endif
#include <terra/json/json.h>
#include "unicode_constants.h"

namespace Terra::JSON
{

namespace
{

/*
 *  ConvertHexCharToInt()
 *
 *  Description:
 *      This is a utility function that will convert a single hex character
 *      to an unsigned integer value.
 *
 *  Parameters:
 *      hex_digit [in]
 *          A single hex digit.
 *
 *  Returns:
 *      The integer value corresponding to the provided hex digit.
 *
 *  Comments:
 *      None.
 */
constexpr std::uint8_t ConvertHexCharToInt(char hex_digit)
{
    // Get the numeric value of the digit character
    if ((hex_digit >= '0') && (hex_digit <= '9'))
    {
        return hex_digit - '0';
    }

    // Get the numeric value of the alphabetic character
    if ((hex_digit >= 'a') && (hex_digit <= 'f'))
    {
        return hex_digit - 'a' + 10;
    }

    // Get the numeric value of the alphabetic character
    if ((hex_digit >= 'A') && (hex_digit <= 'F'))
    {
        return hex_digit - 'A' + 10;
    }

    throw std::invalid_argument("Invalid hex digit");
}

/*
 *  ConvertHexStringToInt()
 *
 *  Description:
 *      This is a utility function that will convert a four character hex
 *      string into an unsigned integer.
 *
 *  Parameters:
 *      hex_string [in]
 *          A four-character string holding hex digits to convert to an integer.
 *
 *  Returns:
 *      The integer value corresponding to the provided hex digits.
 *
 *  Comments:
 *      None.
 */
std::uint32_t ConvertHexStringToInt(const std::string &hex_string)
{
    // The hex string must be exactly four octets, else throw an exception
    if (hex_string.length() != 4)
    {
        throw std::invalid_argument("Invalid hex string length");
    }

    // Convert each digit, shifting 4 bits to accommodate the next digit
    std::uint32_t value = ConvertHexCharToInt(hex_string[0]);
    value <<= 4;
    value |= ConvertHexCharToInt(hex_string[1]);
    value <<= 4;
    value |= ConvertHexCharToInt(hex_string[2]);
    value <<= 4;
    value |= ConvertHexCharToInt(hex_string[3]);

    return value;
}

/*
 *  ParsingErrorString()
 *
 *  Description:
 *      Produce a consistently formatted JSON parsing error string.
 *
 *  Parameters:
 *      line [in]
 *          Current line number.
 *
 *      column [in]
 *          Current column number.
 *
 *      text [in]
 *          Text to print related to the parsing error.
 *
 *  Returns:
 *      The resulting error string.
 *
 *  Comments:
 *      None.
 */
std::string ParsingErrorString(std::size_t line,
                               std::size_t column,
                               std::string text)
{
#ifdef __cpp_lib_format
    return std::format("JSON parsing error at line {}, column {}: {}",
                       line,
                       column,
                       text);
#else
    std::ostringstream oss;
    oss << "JSON parsing error at line " << line << ", column " << column
        << ": " << text;
    return oss.str();
#endif
}

} // namespace

/*
 *  JSONParser::Parse()
 *
 *  Description:
 *      Function to parse the given input span and return a JSON object.
 *
 *  Parameters:
 *      content [in]
 *          The content to parse when generating a JSON object.  The content
 *          is assumed to be UTF-8 text.  If the character encoding MUST be
 *          in UTF-8.
 *
 *  Returns:
 *      A JSON object containing the parsed JSON content.  If there is an
 *      error parsing the content, an exception will be thrown.
 *
 *  Comments:
 *      None.
 */
JSON JSONParser::Parse(const std::string_view content)
{
    return Parse(
        std::u8string_view(reinterpret_cast<const char8_t *>(content.data()),
                           content.length()));
}

/*
 *  JSONParser::Parse()
 *
 *  Description:
 *      Function to parse the given input span and return a JSON object.
 *
 *  Parameters:
 *      content [in]
 *          The content to parse when generating a JSON object.  The content
 *          is assumed to be UTF-8 text.  If the character encoding MUST be
 *          in UTF-8.
 *
 *  Returns:
 *      A JSON object containing the parsed JSON content.  If there is an
 *      error parsing the content, an exception will be thrown.
 *
 *  Comments:
 *      None.
 */
JSON JSONParser::Parse(const std::u8string_view content)
{
    // Ensure the content is not empty
    if (content.empty()) throw JSONException("The content string is empty");

    // Initialize the parsing context variables
    p = content.data();
    q = content.data() + content.size();
    line = 0;
    column = 0;

    // Skip over whitespace
    ConsumeWhitespace();

    // Ensure there is still data to consider
    if (EndOfInput())
    {
        throw JSONException("The content string contains only whitespace");
    }

    // Create a JSON object holding the expected type
    JSON json(ParseInitialValue());

    // Consume any trailing whitespace
    ConsumeWhitespace();

    // Ensure all input is consumed
    if (!EndOfInput())
    {
        throw JSONException(
            ParsingErrorString(line, column, "Unexpected character"));
    }

    return json;
}

/*
 *  JSONParser::ConsumeWhitespace()
 *
 *  Description:
 *      Consume whitespace characters until the first non-whitespace character
 *      or the end of input is reached.
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
void JSONParser::ConsumeWhitespace()
{
    // Iterate over input until the end of input
    while (!EndOfInput())
    {
        // Is the present character whitespace?
        if ((*p == ' ') || (*p == '\r') || (*p == '\t'))
        {
            AdvanceReadPosition();
            continue;
        }

        // Also whitespace, but the line count increments
        if (*p == '\n')
        {
            AdvanceReadPosition();
            column = 0;
            continue;
        }

        break;
    }
}

/*
 *  JSONParser::DetermineValueType()
 *
 *  Description:
 *      Inspect the current octet to determine the expected JSON value type.
 *      The parser is assumed to be at the start of a new value.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      The type of value as determined by inspecting the current octet.
 *
 *  Comments:
 *      The initial character can reveal the type of data that should follow:
 *          " - string
 *          [ - array
 *          { - object
 *          0 to 9 or minus - number
 *          t - true
 *          f - false
 *          n - null
 */
JSONValueType JSONParser::DetermineValueType() const
{
    JSONValueType value_type{};

    // Do not read beyond the buffer
    if (EndOfInput())
    {
        throw JSONException(
            ParsingErrorString(line, column, "Incomplete JSON text"));
    }

    switch (*p)
    {
        case '"':
            value_type = JSONValueType::String;
            break;

        case '[':
            value_type = JSONValueType::Array;
            break;

        case '{':
            value_type = JSONValueType::Object;
            break;

        case 't':
            [[fallthrough]];

        case 'f':
            [[fallthrough]];

        case 'n':
            value_type = JSONValueType::Literal;
            break;

        case '-':
            value_type = JSONValueType::Number;
            break;

        default:
            if (std::isdigit(*p) != 0)
            {
                value_type = JSONValueType::Number;
            }
            else
            {
                throw JSONException(ParsingErrorString(line,
                                                       column,
                                                       "Unknown value type"));
            }
            break;
    }

    return value_type;
}

/*
 *  JSONParser::ParseInitialValue()
 *
 *  Description:
 *      This function will parse the initial data type. It will inspect the
 *      input stream and then parse the data and following data accordingly.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A JSON object holding the parsed value type.  An exception will be
 *      thrown if there is a parsing error.
 *
 *  Comments:
 *      None.
 */
JSONValue JSONParser::ParseInitialValue()
{
    // Determine the value type
    JSONValueType value_type = DetermineValueType();

    // If the initial value is an object, put one on the context and handle it
    if (value_type == JSONValueType::Object)
    {
        JSONValue json_object = JSONObject();

        // Put reference into the parsing context, initializing elements
        composite_context.emplace_back(&json_object, false, false, false);

        // Parse the element in the composite context
        ParseCompositeValue();

        // Upon return, the context should be empty
        if (!composite_context.empty())
        {
            throw JSONException("Error parsing composite type");
        }

        return json_object;
    }

    // If the initial value is an array, put one on the context and handle it
    if (value_type == JSONValueType::Array)
    {
        JSONValue json_array = JSONArray();

        // Put reference into the parsing context, initializing elements
        composite_context.emplace_back(&json_array, false, false, false);

        // Parse the element in the composite context
        ParseCompositeValue();

        // Upon return, the context should be empty
        if (!composite_context.empty())
        {
            throw JSONException("Error parsing composite type");
        }

        return json_array;
    }

    // Return the parsed primitive type
    return ParsePrimitiveValue(value_type);
}

/*
 *  JSONParser::ParsePrimitiveValue()
 *
 *  Description:
 *      This function will parse the next primitive value of the given type,
 *      returning a JSON object of the specified type.  The caller of this
 *      function should have verified that the upcoming text contains the
 *      specified type.  That would be done by first calling the function
 *      DetermineValueType().
 *
 *      A primitive type is a string, number, or literal.
 *
 *  Parameters:
 *      value_type [in]
 *          The type of next value type to assume when parsing.
 *
 *  Returns:
 *      A JSON object holding the parsed value type.  An exception will be
 *      thrown if there is a parsing error.
 *
 *  Comments:
 *      None.
 */
JSONValue JSONParser::ParsePrimitiveValue(JSONValueType value_type)
{
    // Each distinct type requires entirely different parsing logic
    switch (value_type)
    {
        case JSONValueType::String:
            return ParseString();
            break;

        case JSONValueType::Number:
            return ParseNumber();
            break;

        case JSONValueType::Literal:
            return ParseLiteral();
            break;

        case JSONValueType::Object:
            [[fallthrough]];

        case JSONValueType::Array:
            throw JSONException("Unexpected composite type");
            break;

        default:
            throw JSONException("Unknown value type provided");
    };
}

/*
 *  JSONParser::ParseCompositeValue()
 *
 *  Description:
 *      This function will parse a composite value based on the type of value
 *      found at the back of the composite_context vector.
 *
 *      A composite type is an array or object.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing, though an exception will be thrown if there is an error
 *      consuming the composite_context data.
 *
 *  Comments:
 *      None.
 */
void JSONParser::ParseCompositeValue()
{
    // Ensure composite_context is not empty
    if (composite_context.empty())
    {
        throw JSONException("Composite context unexpectedly empty");
    }

    // Loop until the composite vector is fully consumed
    while (!composite_context.empty())
    {
        // If the back element is a JSON object, parse it
        if (std::holds_alternative<JSONObject>(*composite_context.back().value))
        {
            // Process the JSON object
            ParseObject();

            // If we saw the closing brace or bracket, we're done with
            // this element
            if (composite_context.back().closing_seen)
            {
                composite_context.pop_back();
            }

            // Continue the loop iteration
            continue;
        }

        // If the back element is a JSON array, parse it
        if (std::holds_alternative<JSONArray>(*composite_context.back().value))
        {
            // Process the JSON array
            ParseArray();

            // If we saw the closing brace or bracket, we're done with
            // this element
            if (composite_context.back().closing_seen)
            {
                composite_context.pop_back();
            }

            // Continue the loop iteration
            continue;
        }

        throw JSONException("Unexpected type in composite context");
    }
}

/*
 *  JSONParser::ParseString()
 *
 *  Description:
 *      This function assumes the subsequent input is a JSON string and will
 *      return a JSON object holding that type.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A JSON object holding the parsed value type.  An exception will be
 *      thrown if there is a parsing error.
 *
 *  Comments:
 *      It is assumed the read position is at the start of the string without
 *      leading whitespace.
 */
JSONString JSONParser::ParseString()
{
    bool handle_escape = false;
    bool close_quote = false;
    JSONString json_string;

    // Do not read beyond the buffer
    if (EndOfInput())
    {
        throw JSONException(
            ParsingErrorString(line, column, "Incomplete JSON text"));
    }

    // The first octet should be double quote
    if (*p != '"')
    {
        throw JSONException(
            ParsingErrorString(line, column, "Expected leading quote mark"));
    }

    // Advance the parsing position
    AdvanceReadPosition();

    // Everything else is a part of the string
    while (!EndOfInput())
    {
        // Control characters are not permitted in strings
        if (*p < 0x20)
        {
            throw JSONException(ParsingErrorString(line,
                                                   column,
                                                   "Illegal control character "
                                                   "in string"));
        }

        // If there was an escape on the last iteration, handle it
        if (handle_escape)
        {
            // Inspect escaped character
            switch (*p)
            {
                case 'b':
                    AdvanceReadPosition();
                    json_string.value.push_back('\b');
                    break;

                case 'f':
                    AdvanceReadPosition();
                    json_string.value.push_back('\f');
                    break;

                case 'n':
                    AdvanceReadPosition();
                    json_string.value.push_back('\n');
                    break;

                case 'r':
                    AdvanceReadPosition();
                    json_string.value.push_back('\r');
                    break;

                case 't':
                    AdvanceReadPosition();
                    json_string.value.push_back('\t');
                    break;

                case 'u':
                    AdvanceReadPosition();
                    ParseUnicode(json_string);
                    break;

                default:
                    json_string.value.push_back(*p);
                    AdvanceReadPosition();
            };

            // Done with escape
            handle_escape = false;

            continue;
        }

        // If this is the end of the string, stop processing
        if (*p == '"')
        {
            AdvanceReadPosition();
            close_quote = true;
            break;
        }

        // Is the next character (or characters) escaped?
        if (*p == '\\')
        {
            AdvanceReadPosition();
            handle_escape = true;
            continue;
        }

        // Add the current character to the parsed string
        json_string.value.push_back(*p);

        // Advance the read position
        AdvanceReadPosition();
    }

    // Error if the closing quote was not seen
    if (!close_quote)
    {
        throw JSONException(ParsingErrorString(line,
                                               column,
                                               "No closing quote parsing "
                                               "string"));
    }

    return json_string;
}

/*
 *  JSONParser::ParseUnicode()
 *
 *  Description:
 *      This function will consume input octets and parse one or two
 *      Unicode characters.  Unicode characters are either four hex digits
 *      like "\u5C0F" or eight hex digits like "\uD83D\uDE01".  The latter
 *      is an example of UTF-16 surrogate pair.
 *
 *  Parameters:
 *      json_string [in/out]
 *          The UTF-8 JSON string onto which characters are appended.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      It is assumed that the read position sits at the initial hex digit
 *      of the escaped Unicode sequence (i.e., just beyond '\u').
 *      See the Unicode surrogate documentation here:
 *      https://www.Unicode.org/faq/utf_bom.html#utf16-3
 *      https://en.wikipedia.org/wiki/UTF-16#U+D800_to_U+DFFF_(surrogates)
 */
void JSONParser::ParseUnicode(JSONString &json_string)
{
    std::uint32_t code_value{};
    std::size_t initial_column = column;

    // Ensure there are at least 4 octets to consume
    if (RemainingInput() < 4)
    {
        throw JSONException(ParsingErrorString(line,
                                               column,
                                               "Insufficient input following "
                                               "\\u sequence"));
    }

    // Extract the hex digit string
    std::string hex_digits(4, ' ');
    hex_digits[0] = static_cast<char>(p[0]);
    hex_digits[1] = static_cast<char>(p[1]);
    hex_digits[2] = static_cast<char>(p[2]);
    hex_digits[3] = static_cast<char>(p[3]);
    AdvanceReadPosition(4);

    // Get the value of the hex string
    try
    {
        code_value = ConvertHexStringToInt(hex_digits);
    }
    catch (const std::invalid_argument &e)
    {
        throw JSONException(ParsingErrorString(line, column - 4, e.what()));
    }

    // Is this code in the surrogate range?
    if ((code_value >= Unicode::Surrogate_High_Min) &&
        (code_value <= Unicode::Surrogate_Low_Max))
    {
        std::uint32_t low_code_value{};

        // Ensure the code value is not in the low surrogate range
        if ((code_value >= Unicode::Surrogate_Low_Min) &&
            (code_value <= Unicode::Surrogate_Low_Max))
        {
            throw JSONException(ParsingErrorString(line,
                                                   column - 6,
                                                   "Unexpected low Unicode "
                                                   "surrogate found"));
        }

        // Ensure there are at least 6 octets to consume ('\uNNNN')
        if (RemainingInput() < 6)
        {
            throw JSONException(ParsingErrorString(line,
                                                   column,
                                                   "Insufficient input "
                                                   "following high Unicode "
                                                   "surrogate"));
        }

        // The following characters should be '\uNNNN' where 'N' is hex
        if ((p[0] != '\\') || (p[1] != 'u'))
        {
            throw JSONException(ParsingErrorString(line,
                                                   column,
                                                   "Expected low Unicode "
                                                   "surrogate, but did not "
                                                   "find one"));
        }

        // Advance over '\u'
        p += 2;
        column += 2;

        // Extract the hex digit string
        hex_digits[0] = static_cast<char>(p[0]);
        hex_digits[1] = static_cast<char>(p[1]);
        hex_digits[2] = static_cast<char>(p[2]);
        hex_digits[3] = static_cast<char>(p[3]);
        AdvanceReadPosition(4);

        // Get the value of the hex string
        try
        {
            low_code_value = ConvertHexStringToInt(hex_digits);
        }
        catch (const std::invalid_argument &e)
        {
            throw JSONException(
                ParsingErrorString(line, column - 4, e.what()));
        }

        // Ensure the low surrogate value is within the expected range
        if ((low_code_value < Unicode::Surrogate_Low_Min) ||
            (low_code_value > Unicode::Surrogate_Low_Max))
        {
            throw JSONException(ParsingErrorString(line,
                                                   column - 6,
                                                   "Expected low Unicode "
                                                   "surrogate value"));
        }

        // Convert the high / low code point values to a UTF-32 value
        // (See: https://www.Unicode.org/faq/utf_bom.html#utf16-3)
        code_value =
            (code_value << 10) + low_code_value + Unicode::Surrogate_Offset;
    }

    // The following will produce the UTF-8 code point(s)
    // (See: https://www.rfc-editor.org/rfc/rfc3629#section-3)
    if (code_value <= 0x7f)
    {
        // 0nnnnnn
        json_string.value.push_back(static_cast<char8_t>(code_value));
        return;
    }

    if (code_value <= 0x7ff)
    {
        // 110nnnnn 10nnnnnn
        json_string.value.push_back(0xc0 | ((code_value >> 6) & 0x1f));
        json_string.value.push_back(0x80 | ((code_value     ) & 0x3f));
        return;
    }

    if (code_value <= 0xffff)
    {
        // 1110nnnn 10nnnnnn 10nnnnnn
        json_string.value.push_back(0xe0 | ((code_value >> 12) & 0x0f));
        json_string.value.push_back(0x80 | ((code_value >>  6) & 0x3f));
        json_string.value.push_back(0x80 | ((code_value      ) & 0x3f));
        return;
    }

    if (code_value <= 0x10ffff)
    {
        // 11110nnn 10nnnnnn 10nnnnnn 10nnnnnn
        json_string.value.push_back(0xf0 | ((code_value >> 18) & 0x07));
        json_string.value.push_back(0x80 | ((code_value >> 12) & 0x3f));
        json_string.value.push_back(0x80 | ((code_value >>  6) & 0x3f));
        json_string.value.push_back(0x80 | ((code_value      ) & 0x3f));
        return;
    }

    // It should actually be impossible to get to this point given prior
    // checks, but this code is here out of an abundance of caution
    throw JSONException(ParsingErrorString(line,
                                           initial_column,
                                           "Unicode value is invalid"));
}

/*
 *  JSONParser::ParseNumber()
 *
 *  Description:
 *      This function assumes the subsequent input is a JSON number and will
 *      return a JSONNumber object holding the parsed value.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A JSON object holding the parsed value type.  An exception will be
 *      thrown if there is a parsing error.
 *
 *  Comments:
 *      It is assumed the read position is at the start of the string without
 *      leading whitespace.
 */
JSONNumber JSONParser::ParseNumber()
{
    enum class NumberState : std::uint8_t
    {
        Sign,
        Integer,
        Float,
        ExponentSign,
        Exponent,
    };
    JSONNumber json_number;
    std::string number;
    bool valid_number = false;
    bool end_of_number = false;
    bool is_float = false;

    // Do not read beyond the buffer
    if (EndOfInput())
    {
        throw JSONException(
            ParsingErrorString(line, column, "Incomplete JSON number"));
    }

    // Initial parsing state is Sign
    NumberState state = NumberState::Sign;

    // Everything else is a part of the string
    while (!EndOfInput() && !end_of_number)
    {
        switch (state)
        {
            case NumberState::Sign:
                // Do we have a sign octet?
                if (*p == '-')
                {
                    number.push_back(static_cast<char>(*p));
                    AdvanceReadPosition();
                    state = NumberState::Integer;
                    break;
                }

                if (std::isdigit(*p) != 0)
                {
                    state = NumberState::Integer;
                    break;
                }

                // Any other character suggests we have parsed the whole number
                end_of_number = true;

                break;

            case NumberState::Integer:
                if (std::isdigit(*p) != 0)
                {
                    number.push_back(static_cast<char>(*p));
                    AdvanceReadPosition();
                    valid_number = true;
                    break;
                }

                if (*p == '.')
                {
                    if (!valid_number)
                    {
                        throw JSONException(ParsingErrorString(line,
                                                               column,
                                                               "Invalid "
                                                               "number"));
                    }
                    number.push_back(static_cast<char>(*p));
                    AdvanceReadPosition();
                    valid_number = false;
                    is_float = true;
                    state = NumberState::Float;
                    break;
                }

                if ((*p == 'e') || (*p == 'E'))
                {
                    if (!valid_number)
                    {
                        throw JSONException(ParsingErrorString(line,
                                                               column,
                                                               "Invalid "
                                                               "number"));
                    }
                    number.push_back(static_cast<char>(*p));
                    AdvanceReadPosition();
                    is_float = true;
                    state = NumberState::ExponentSign;
                    valid_number = false;
                    break;
                }

                // Any other character suggests we have parsed the whole number
                end_of_number = true;

                break;

            case NumberState::Float:
                if (std::isdigit(*p) != 0)
                {
                    number.push_back(static_cast<char>(*p));
                    AdvanceReadPosition();
                    valid_number = true;
                    break;
                }

                if ((*p == 'e') || (*p == 'E'))
                {
                    if (!valid_number)
                    {
                        throw JSONException(ParsingErrorString(line,
                                                               column,
                                                               "Invalid "
                                                               "number"));
                    }
                    number.push_back(static_cast<char>(*p));
                    AdvanceReadPosition();
                    state = NumberState::ExponentSign;
                    valid_number = false;
                    break;
                }

                // Any other character suggests we have parsed the whole number
                end_of_number = true;

                break;

            case NumberState::ExponentSign:
                // Do we have a sign octet?
                if ((*p == '-') || (*p == '+'))
                {
                    number.push_back(static_cast<char>(*p));
                    AdvanceReadPosition();
                    state = NumberState::Exponent;
                    break;
                }

                if (std::isdigit(*p) != 0)
                {
                    state = NumberState::Exponent;
                    break;
                }

                // Any other character suggests we have parsed the whole number
                end_of_number = true;

                break;

            case NumberState::Exponent:
                if (std::isdigit(*p) != 0)
                {
                    number.push_back(static_cast<char>(*p));
                    AdvanceReadPosition();
                    valid_number = true;
                    break;
                }

                // Any other character suggests we have parsed the whole number
                end_of_number = true;

                break;
        }
    }

    // If we do not have a valid number, throw an exception
    if (!valid_number)
    {
        throw JSONException(ParsingErrorString(line, column, "Invalid number"));
    }

    // Convert the string to a JSON
    try
    {
        if (is_float)
        {
            *json_number = stod(number);
        }
        else
        {
            *json_number = stoll(number);
        }
    }
    catch (const std::exception &e)
    {
        throw JSONException(ParsingErrorString(
            line,
            column,
            std::string("Failed converting number: ") + e.what()));
    }
    catch (...)
    {
        throw JSONException(ParsingErrorString(
            line,
            column,
            std::string("Failed converting number")));
    }

    return json_number;
}

/*
 *  JSONParser::ParseObject()
 *
 *  Description:
 *      This function will begin processing a JSON object or, if the opening
 *      brace is already seen, continue processing.  This function behaves
 *      differently than other JSON types since objects can be nested.
 *      While this function takes no input parameters, it operates on the
 *      member vector composite_context to maintain state.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      It is assumed the read position is at the start of the object without
 *      leading whitespace.
 */
void JSONParser::ParseObject()
{
    // Ensure the parsing context is not empty
    if (composite_context.empty())
    {
        throw JSONException("Composite context unexpectedly empty");
    }

    // Ensure the back of the context is a JSONObject reference
    if (!std::holds_alternative<JSONObject>(*composite_context.back().value))
    {
        throw JSONException("Unexpected type in composite context");
    }

    // Assign values from the context members (as references)
    auto &json_object = std::get<JSONObject>(*composite_context.back().value);
    bool &opening_seen = composite_context.back().opening_seen;
    bool &closing_seen = composite_context.back().closing_seen;
    bool &member_seen = composite_context.back().member_seen;

    // Do not read beyond the buffer
    if (EndOfInput())
    {
        throw JSONException(
            ParsingErrorString(line, column, "Incomplete JSON object"));
    }

    // Have the opening brace for this object been seen?
    if (!opening_seen)
    {
        // The first octet should be an open brace
        if (*p != '{')
        {
            throw JSONException(
                ParsingErrorString(line, column, "Expected leading brace"));
        }

        // Note that the opening brace has been seen
        opening_seen = true;

        // Advance the parsing position
        AdvanceReadPosition();
    }

    // Everything else is a part of the JSON object
    while (!closing_seen && !EndOfInput())
    {
        // Skip over any whitespace
        ConsumeWhitespace();

        // Ensure we're not at the end of input
        if (EndOfInput()) break;

        // Check if this is the end of the object
        if (*p == '}')
        {
            AdvanceReadPosition();
            closing_seen = true;
            break;
        }

        // If the first member was seen, we should be at a comma
        if (member_seen)
        {
            // Ensure we see the next member is separated by a comma
            if (*p != ',')
            {
                throw JSONException(
                    ParsingErrorString(line, column, "Expected a comma"));
            }

            // Advance the parsing position
            AdvanceReadPosition();

            // Skip over any whitespace
            ConsumeWhitespace();

            // Ensure we're not at the end of input
            if (EndOfInput()) break;

            // Ensure this is not an out-of-place closing brace
            if (*p == '}')
            {
                throw JSONException(ParsingErrorString(line,
                                                       column,
                                                       "Premature end of JSON "
                                                       "object"));
            }
        }

        // We should find a key / value pair, so check the key type
        JSONValueType key_type = DetermineValueType();

        // This should be a string
        if (key_type != JSONValueType::String)
        {
            throw JSONException(
                ParsingErrorString(line, column, "Expected a string"));
        }

        // Parse the string for the name value
        JSONString name = ParseString();

        // Ensure this name is not already in use
        if (json_object.HasKey(*name))
        {
            throw JSONException(
                ParsingErrorString(line, column, "Duplicate name"));
        }

        // Consume any whitespace
        ConsumeWhitespace();

        // Ensure we're not at the end of input
        if (EndOfInput()) break;

        // Next, there should be a : separator
        if (*p != ':')
        {
            throw JSONException(
                ParsingErrorString(line, column, "Expected a string"));
        }

        // Advance the read position
        AdvanceReadPosition();

        // Consume any whitespace
        ConsumeWhitespace();

        // Ensure we're not at the end of input
        if (EndOfInput()) break;

        // Note that the first member was seen
        member_seen = true;

        // What is the type of data to consume?
        JSONValueType value_type = DetermineValueType();

        // If the next type is an JSONObject type, create that type and put
        // it into the parsing context
        if (value_type == JSONValueType::Object)
        {
            json_object.value[*name] = JSONObject();
            composite_context.emplace_back(
                &(*json_object.value[*name]),
                false,
                false,
                false);
            return;
        }

        // If the next type is an JSONObject type, create that type and put
        // it into the parsing context
        if (value_type == JSONValueType::Array)
        {
            json_object.value[*name] = JSONArray();
            composite_context.emplace_back(
                &(*json_object.value[*name]),
                false,
                false,
                false);
            return;
        }

        // Parse the primitive JSON value that follows, placing it into the map
        json_object.value[*name] = ParsePrimitiveValue(value_type);
    }

    // Ensure the closing brace was seen
    if (!closing_seen)
    {
        throw JSONException(
            ParsingErrorString(line, column, "Unexpected end of JSON object"));
    }
}

/*
 *  JSONParser::ParseArray()
 *
 *  Description:
 *      This function assumes the subsequent input is a JSON array and will
 *      return a JSONArray type.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A JSONArray holding the parsed value.  An exception will be
 *      thrown if there is a parsing error.
 *
 *  Comments:
 *      It is assumed the read position is at the start of the array without
 *      leading whitespace.
 */
void JSONParser::ParseArray()
{
    // Ensure the parsing context is not empty
    if (composite_context.empty())
    {
        throw JSONException("Composite context unexpectedly empty");
    }

    // Ensure the back of the context is a JSONObject reference
    if (!std::holds_alternative<JSONArray>(*composite_context.back().value))
    {
        throw JSONException("Unexpected type in composite context");
    }

    // Assign values from the context members (as references)
    auto &json_array = std::get<JSONArray>(*composite_context.back().value);
    bool &opening_seen = composite_context.back().opening_seen;
    bool &closing_seen = composite_context.back().closing_seen;
    bool &member_seen = composite_context.back().member_seen;

    // Do not read beyond the buffer
    if (EndOfInput())
    {
        throw JSONException(
            ParsingErrorString(line, column, "Incomplete JSON array"));
    }

    // Have the opening bracket for this object been seen?
    if (!opening_seen)
    {
        // The first octet should be an open bracket
        if (*p != '[')
        {
            throw JSONException(
                ParsingErrorString(line, column, "Expected leading bracket"));
        }

        // Indicate that the opening bracket was seen
        opening_seen = true;

        // Advance the parsing position
        AdvanceReadPosition();
    }

    // Everything else is a part of the JSON array
    while (!closing_seen && !EndOfInput())
    {
        // Skip over any whitespace
        ConsumeWhitespace();

        // Ensure we're not at the end of input
        if (EndOfInput()) break;

        // Check if this is the end of the array
        if (*p == ']')
        {
            AdvanceReadPosition();
            closing_seen = true;
            break;
        }

        // If the first member was seen, we should be at a comma
        if (member_seen)
        {
            // Ensure we see the next member is separated by a comma
            if (*p != ',')
            {
                throw JSONException(
                    ParsingErrorString(line, column, "Expected a comma"));
            }

            // Advance the parsing position
            AdvanceReadPosition();

            // Skip over any whitespace
            ConsumeWhitespace();

            // Ensure we're not at the end of input
            if (EndOfInput()) break;

            // Ensure this is not an out-of-place closing bracket
            if (*p == ']')
            {
                throw JSONException(ParsingErrorString(line,
                                                       column,
                                                       "Premature end of JSON "
                                                       "array"));
            }
        }

        // Note that the first member was seen
        member_seen = true;

        // What is the type of data to consume?
        JSONValueType value_type = DetermineValueType();

        // If the next type is an JSONObject type, create that type and put
        // it into the parsing context
        if (value_type == JSONValueType::Object)
        {
            json_array.value.emplace_back(JSONObject());
            composite_context.emplace_back(
                &(*json_array.value.back()),
                false,
                false,
                false);
            return;
        }

        // If the next type is an JSONObject type, create that type and put
        // it into the parsing context
        if (value_type == JSONValueType::Array)
        {
            json_array.value.emplace_back(JSONArray());
            composite_context.emplace_back(
                &(*json_array.value.back()),
                false,
                false,
                false);
            return;
        }

        // Parse the JSON value that follows, placing it into the map
        json_array.value.emplace_back(ParsePrimitiveValue(value_type));
    }

    // Ensure the closing brace was seen
    if (!closing_seen)
    {
        throw JSONException(
            ParsingErrorString(line, column, "Unexpected end of JSON array"));
    }
}

/*
 *  JSONParser::ParseLiteral()
 *
 *  Description:
 *      This function assumes the subsequent input is a JSON literal and will
 *      return a JSONLiteral type.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A JSONLiteral holding the parsed value.  An exception will be
 *      thrown if there is a parsing error.
 *
 *  Comments:
 *      It is assumed the read position is at the start of the literal without
 *      leading whitespace.
 */
JSONLiteral JSONParser::ParseLiteral()
{
    // Do not read beyond the buffer
    if (EndOfInput())
    {
        throw JSONException(ParsingErrorString(line,
                                               column,
                                               "Incomplete JSON text"));
    }

    // Determine the type of literal
    switch (*p)
    {
        case 'f':
            if (RemainingInput() < 5) break;
            if ((p[0] == 'f') && (p[1] == 'a') && (p[2] == 'l') &&
                (p[3] == 's') && (p[4] == 'e'))
            {
                p += 5;
                column += 5;
                return JSONLiteral::False;
            }
            break;

        case 't':
            if (RemainingInput() < 4) break;
            if ((p[0] == 't') && (p[1] == 'r') && (p[2] == 'u') &&
                (p[3] == 'e'))
            {
                p += 4;
                column += 4;
                return JSONLiteral::True;
            }
            break;

        case 'n':
            if (RemainingInput() < 4) break;
            if ((p[0] == 'n') && (p[1] == 'u') && (p[2] == 'l') &&
                (p[3] == 'l'))
            {
                p += 4;
                column += 4;
                return JSONLiteral::Null;
            }
            break;

        default:
            break;
    }

    throw JSONException(
        ParsingErrorString(line, column, "Unknown JSON literal"));
}

} // namespace Terra::JSON
