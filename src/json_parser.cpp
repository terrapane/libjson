/*
 *  json_parser.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the JSON parsing and serialization object.
 *
 *  Portability Issues:
 *      None.
 */

#include <format>
#include <cctype>
#include <terra/json/json.h>
#include "unicode_constants.h"

namespace Terra::JSON
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
static std::uint32_t ConvertHexStringToInt(const std::string &hex_string)
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
static std::string ParsingErrorString(std::size_t line,
                                      std::size_t column,
                                      std::string text)
{
    return std::format("JSON parsing error at line {}, column {}: {}",
                       line,
                       column,
                       text);
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
    JSON json(ParseValue(DetermineValueType()));

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
    JSONValueType value_type;

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
            if (std::isdigit(*p))
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
 *  JSONParser::ParseValue()
 *
 *  Description:
 *      This function will parse the next single value of the given type,
 *      returning a JSON object of the specified type.  The caller of this
 *      function should have verified that the upcoming text contains the
 *      specified type.  That would be done by first calling the function
 *      DetermineValueType().
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
JSONValue JSONParser::ParseValue(JSONValueType value_type)
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

        case JSONValueType::Object:
            return ParseObject();
            break;

        case JSONValueType::Array:
            return ParseArray();
            break;

        case JSONValueType::Literal:
            return ParseLiteral();
            break;

        default:
            throw JSONException("Unknown value type provided");
    };
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
    std::uint32_t code_value;
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
    hex_digits[0] = p[0];
    hex_digits[1] = p[1];
    hex_digits[2] = p[2];
    hex_digits[3] = p[3];
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
        std::uint32_t low_code_value;

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
        std::string hex_digits(4, ' ');
        hex_digits[0] = p[0];
        hex_digits[1] = p[1];
        hex_digits[2] = p[2];
        hex_digits[3] = p[3];
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
        if (!((low_code_value >= Unicode::Surrogate_Low_Min) &&
              (low_code_value <= Unicode::Surrogate_Low_Max)))
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
    enum class NumberState {
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
                    number.push_back(*p);
                    AdvanceReadPosition();
                    state = NumberState::Integer;
                    break;
                }

                if (std::isdigit(*p))
                {
                    state = NumberState::Integer;
                    break;
                }

                // Any other character suggests we have parsed the whole number
                end_of_number = true;

                break;

            case NumberState::Integer:
                if (std::isdigit(*p))
                {
                    number.push_back(*p);
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
                    number.push_back(*p);
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
                    number.push_back(*p);
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
                if (std::isdigit(*p))
                {
                    number.push_back(*p);
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
                    number.push_back(*p);
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
                    number.push_back(*p);
                    AdvanceReadPosition();
                    state = NumberState::Exponent;
                    break;
                }

                if (std::isdigit(*p))
                {
                    state = NumberState::Exponent;
                    break;
                }

                // Any other character suggests we have parsed the whole number
                end_of_number = true;

                break;

            case NumberState::Exponent:
                if (std::isdigit(*p))
                {
                    number.push_back(*p);
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
            json_number.value = stod(number);
        }
        else
        {
            json_number.value = stoll(number);
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
 *      This function assumes the subsequent input is a JSON object and will
 *      return a JSONObject type.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A JSONObject holding the parsed value.  An exception will be
 *      thrown if there is a parsing error.
 *
 *  Comments:
 *      It is assumed the read position is at the start of the object without
 *      leading whitespace.
 */
JSONObject JSONParser::ParseObject()
{
    JSONObject json_object;
    bool closing_brace_seen = false;
    bool first_member_seen = false;

    // Do not read beyond the buffer
    if (EndOfInput())
    {
        throw JSONException(
            ParsingErrorString(line, column, "Incomplete JSON object"));
    }

    // The first octet should be an open brace
    if (*p != '{')
    {
        throw JSONException(
            ParsingErrorString(line, column, "Expected leading brace"));
    }

    // Advance the parsing position
    AdvanceReadPosition();

    // Everything else is a part of the JSON object
    while (!EndOfInput())
    {
        // Skip over any whitespace
        ConsumeWhitespace();

        // Ensure we're not at the end of input
        if (EndOfInput()) break;

        // Check if this is the end of the object
        if (*p == '}')
        {
            AdvanceReadPosition();
            closing_brace_seen = true;
            break;
        }

        // If the first member was seen, we should be at a comma
        if (first_member_seen == true)
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

        // Determine the type of the initial value
        auto value_type = DetermineValueType();

        // This should be a string
        if (value_type != JSONValueType::String)
        {
            throw JSONException(
                ParsingErrorString(line, column, "Expected a string"));
        }

        // Parse the string for the name value
        JSONString name = ParseString();

        // Ensure this name is not already in use
        if (json_object.value.find(name.value) != json_object.value.end())
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

        // Parse the JSON value that follows, placing it into the map
        json_object.value[name.value] = ParseValue(DetermineValueType());

        // Note that the first member was seen
        first_member_seen = true;
    }

    // Ensure the closing brace was seen
    if (!closing_brace_seen)
    {
        throw JSONException(
            ParsingErrorString(line, column, "Unexpected end of JSON object"));
    }

    return json_object;
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
JSONArray JSONParser::ParseArray()
{
    JSONArray json_array;
    bool closing_bracket_seen = false;
    bool first_member_seen = false;

    // Do not read beyond the buffer
    if (EndOfInput())
    {
        throw JSONException(
            ParsingErrorString(line, column, "Incomplete JSON array"));
    }

    // The first octet should be an open brace
    if (*p != '[')
    {
        throw JSONException(
            ParsingErrorString(line, column, "Expected leading bracket"));
    }

    // Advance the parsing position
    AdvanceReadPosition();

    // Everything else is a part of the JSON object
    while (!EndOfInput())
    {
        // Skip over any whitespace
        ConsumeWhitespace();

        // Ensure we're not at the end of input
        if (EndOfInput()) break;

        // Check if this is the end of the object
        if (*p == ']')
        {
            AdvanceReadPosition();
            closing_bracket_seen = true;
            break;
        }

        // If the first member was seen, we should be at a comma
        if (first_member_seen == true)
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

        // Parse the JSON value that follows, placing it into the map
        json_array.value.emplace_back(ParseValue(DetermineValueType()));

        // Note that the first member was seen
        first_member_seen = true;
    }

    // Ensure the closing brace was seen
    if (!closing_bracket_seen)
    {
        throw JSONException(
            ParsingErrorString(line, column, "Unexpected end of JSON array"));
    }

    return json_array;
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
