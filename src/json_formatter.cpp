/*
 *  json_formatter.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the JSONFormatter, which will accept JSON
 *      text as input and produce formatted JSON output.  By default, the
 *      JSON output will use two space indentions, but this may be configured
 *      during object construction.
 *
 *      This code is based on the JSONParser, but rather than fully parsing
 *      values, parsing is limited to the extent necessary to produce a
 *      properly formatted output.  If there is an obvious syntax error when
 *      trying to produce the output, an exception will be thrown.  However,
 *      the formatter does not check to ensure that text strings, for example,
 *      contain a properly formatted Unicode characters, that object keys are
 *      unique, etc.
 *
 *      Since the formatter does not create a JSON object, but merely consumes
 *      strings that are assumed to represent JSON text, the order of object
 *      keys is not reordered, but merely reformatted.
 *
 *  Portability Issues:
 *      None.
 */

#include <sstream>
#include <format>
#include <cctype>
#include <terra/json/json.h>
#include "unicode_constants.h"

namespace Terra::JSON
{

namespace
{

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
    return std::format("JSON parsing error at line {}, column {}: {}",
                       line,
                       column,
                       text);
}

} // namespace

/*
 *  JSONFormatter::Print()
 *
 *  Description:
 *      Function to print the given JSON object using formatted output.
 *
 *  Parameters:
 *      json [in]
 *          The JSON object to produce as formatted text.
 *
 *  Returns:
 *      A formatted text string for the given JSON object.
 *
 *  Comments:
 *      None.
 */
std::string JSONFormatter::Print(const JSON &json)
{
    std::ostringstream oss;

    // Produce the unformatted text
    oss << json;

    // Return the string as formatted text
    return Print(oss.str());
}

/*
 *  JSONFormatter::Print()
 *
 *  Description:
 *      Function to print the given input span and return a formatted text
 *      string.
 *
 *  Parameters:
 *      content [in]
 *          The JSON content to reformat.
 *
 *  Returns:
 *      A formatted text string for the given input text.
 *
 *  Comments:
 *      None.
 */
std::string JSONFormatter::Print(const std::string_view content)
{
    return Print(
        std::u8string_view(reinterpret_cast<const char8_t *>(content.data()),
                           content.length()));
}

/*
 *  JSONFormatter::Print()
 *
 *  Description:
 *      Function to print the given input span and return a formatted text
 *      string.
 *
 *  Parameters:
 *      content [in]
 *          The JSON content to reformat.
 *
 *  Returns:
 *      A formatted text string for the given input text.
 *
 *  Comments:
 *      None.
 */
std::string JSONFormatter::Print(const std::u8string_view content)
{
    std::ostringstream oss;

    Print(oss, content);

    return oss.str();
}

/*
 *  JSONFormatter::Print()
 *
 *  Description:
 *      Function to print the given input span and return a formatted text
 *      string.
 *
 *  Parameters:
 *      o [in/out]
 *          Stream onto which the JSON string should be output.
 *
 *      content [in]
 *          The JSON content to reformat.
 *
 *  Returns:
 *      A formatted text string for the given input text.
 *
 *  Comments:
 *      None.
 */
void JSONFormatter::Print(std::ostream &o, const std::string_view content)
{
    Print(o,
          std::u8string_view(reinterpret_cast<const char8_t *>(content.data()),
                             content.length()));
}

/*
 *  JSONFormatter::Print()
 *
 *  Description:
 *      Function to print the given input span and return a formatted text
 *      string.
 *
 *  Parameters:
 *      o [in/out]
 *          Stream onto which the JSON string should be output.
 *
 *      content [in]
 *          The JSON content to reformat.
 *
 *  Returns:
 *      A formatted text string for the given input text.
 *
 *  Comments:
 *      None.
 */
void JSONFormatter::Print(std::ostream &o, const std::u8string_view content)
{
    // Assign the output stream pointer so other functions can use it
    this->o = &o;

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

    // Print the text given the determined data type
    PrintValue(DetermineValueType());

    // Consume any trailing whitespace
    ConsumeWhitespace();

    // Ensure all input is consumed
    if (!EndOfInput())
    {
        throw JSONException(
            ParsingErrorString(line, column, "Unexpected character"));
    }
}

/*
 *  JSONFormatter::ProduceIndentation()
 *
 *  Description:
 *      This function will produce the indentation amount given the current
 *      indentation level.
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
void JSONFormatter::ProduceIndentation()
{
    *o << std::string(current_indention, ' ');
}

/*
 *  JSONFormatter::ConsumeWhitespace()
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
void JSONFormatter::ConsumeWhitespace()
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
 *  JSONFormatter::DetermineValueType()
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
JSONValueType JSONFormatter::DetermineValueType() const
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
 *  JSONFormatter::PrintValue()
 *
 *  Description:
 *      This function will parse and print the next single value of the given
 *      type.  The caller of this function should have verified that the
 *      upcoming text contains the specified type.  That would be done by first
 *      calling the function DetermineValueType().
 *
 *  Parameters:
 *      value_type [in]
 *          The type of next value type to assume when parsing.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void JSONFormatter::PrintValue(JSONValueType value_type)
{
    // Each distinct type requires entirely different parsing logic
    switch (value_type)
    {
        case JSONValueType::String:
            PrintString();
            break;

        case JSONValueType::Number:
            PrintNumber();
            break;

        case JSONValueType::Object:
            PrintObject();
            break;

        case JSONValueType::Array:
            PrintArray();
            break;

        case JSONValueType::Literal:
            PrintLiteral();
            break;

        default:
            throw JSONException("Unknown value type provided");
    };
}

/*
 *  JSONFormatter::PrintString()
 *
 *  Description:
 *      This function assumes the subsequent input is a JSON string and will
 *      parse and print the text value.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      It is assumed the read position is at the start of the string without
 *      leading whitespace.
 */
void JSONFormatter::PrintString()
{
    bool close_quote = false;
    bool handle_escape = false;

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

    // Output the leading quote
    *o << '"';

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
            // Produce the escaped character
            *o << static_cast<char>(*p);

            // Done with escape
            handle_escape = false;

            continue;
        }

        // If this is the end of the string, stop processing
        if (*p == '"')
        {
            *o << '"';
            AdvanceReadPosition();
            close_quote = true;
            break;
        }

        // Is the next character escaped?
        if (*p == '\\')
        {
            *o << '\\';
            AdvanceReadPosition();
            handle_escape = true;
            continue;
        }

        // Output the current character
        *o << static_cast<char>(*p);

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
}

/*
 *  JSONFormatter::PrintNumber()
 *
 *  Description:
 *      This function assumes the subsequent input is a JSON number and will
 *      parse and print the value.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      It is assumed the read position is at the start of the string without
 *      leading whitespace.
 */
void JSONFormatter::PrintNumber()
{
    enum class NumberState : std::uint8_t
    {
        Sign,
        Integer,
        Float,
        ExponentSign,
        Exponent,
    };
    bool valid_number = false;
    bool end_of_number = false;

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
                    *o << static_cast<char>(*p);
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
                    *o << static_cast<char>(*p);
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
                    *o << static_cast<char>(*p);
                    AdvanceReadPosition();
                    valid_number = false;
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
                    *o << static_cast<char>(*p);
                    AdvanceReadPosition();
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
                    *o << static_cast<char>(*p);
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
                    *o << static_cast<char>(*p);
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
                    *o << static_cast<char>(*p);
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
                    *o << static_cast<char>(*p);
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
}

/*
 *  JSONFormatter::PrintObject()
 *
 *  Description:
 *      This function assumes the subsequent input is a JSON object, will parse
 *      the contents, and output a formatted value.
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
void JSONFormatter::PrintObject()
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

    // Output the opening brace
    *o << '{' << std::endl;

    // Increase the indention level
    current_indention += indention;

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
            current_indention -= indention;
            *o << std::endl;
            ProduceIndentation();
            *o << '}';
            AdvanceReadPosition();
            closing_brace_seen = true;
            break;
        }

        // If the first member was seen, we should be at a comma
        if (first_member_seen)
        {
            // Ensure we see the next member is separated by a comma
            if (*p != ',')
            {
                throw JSONException(
                    ParsingErrorString(line, column, "Expected a comma"));
            }

            // Output the comma
            *o << ',' << std::endl;

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

        // Print the string with spacing prepended
        ProduceIndentation();
        PrintString();

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

        // Determine the type of the initial value
        value_type = DetermineValueType();

        // Output the colon character and one space (conditionally)
        if (allman_style && ((value_type == JSONValueType::Array) ||
                             (value_type == JSONValueType::Object)))
        {
            // For the Allman coding style, end the line and indent
            *o << ":" << std::endl;
            ProduceIndentation();
        }
        else
        {
            // Otherwise, one space is introduced
            *o << ": ";
        }

        // Print the JSON value that follows
        PrintValue(value_type);

        // Note that the first member was seen
        first_member_seen = true;
    }

    // Ensure the closing brace was seen
    if (!closing_brace_seen)
    {
        throw JSONException(
            ParsingErrorString(line, column, "Unexpected end of JSON object"));
    }
}

/*
 *  JSONFormatter::PrintArray()
 *
 *  Description:
 *      This function assumes the subsequent input is a JSON array and will
 *      output a formatted value.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      It is assumed the read position is at the start of the array without
 *      leading whitespace.
 */
void JSONFormatter::PrintArray()
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

    // Output the opening bracket
    *o << '[' << std::endl;

    // Increase the indention level
    current_indention += indention;

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
            current_indention -= indention;
            *o << std::endl;
            ProduceIndentation();
            *o << ']';
            AdvanceReadPosition();
            closing_bracket_seen = true;
            break;
        }

        // If the first member was seen, we should be at a comma
        if (first_member_seen)
        {
            // Ensure we see the next member is separated by a comma
            if (*p != ',')
            {
                throw JSONException(
                    ParsingErrorString(line, column, "Expected a comma"));
            }

            // Output the comma
            *o << ',' << std::endl;

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

        // Prepend spaces before outputting the JON value
        ProduceIndentation();

        // Print the JSON value that follows
        PrintValue(DetermineValueType());

        // Note that the first member was seen
        first_member_seen = true;
    }

    // Ensure the closing brace was seen
    if (!closing_bracket_seen)
    {
        throw JSONException(
            ParsingErrorString(line, column, "Unexpected end of JSON array"));
    }
}

/*
 *  JSONFormatter::PrintLiteral()
 *
 *  Description:
 *      This function assumes the subsequent input is a JSON literal and will
 *      output the literal value.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      It is assumed the read position is at the start of the literal without
 *      leading whitespace.
 */
void JSONFormatter::PrintLiteral()
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
                *o << "false";
                return;
            }
            break;

        case 't':
            if (RemainingInput() < 4) break;
            if ((p[0] == 't') && (p[1] == 'r') && (p[2] == 'u') &&
                (p[3] == 'e'))
            {
                p += 4;
                column += 4;
                *o << "true";
                return;
            }
            break;

        case 'n':
            if (RemainingInput() < 4) break;
            if ((p[0] == 'n') && (p[1] == 'u') && (p[2] == 'l') &&
                (p[3] == 'l'))
            {
                p += 4;
                column += 4;
                *o << "null";
                return;
            }
            break;

        default:
            break;
    }

    throw JSONException(
        ParsingErrorString(line, column, "Unknown JSON literal"));
}

} // namespace Terra::JSON
