/*
 *  json_string.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file contains implementation of some functions defined for the
 *      JSONString object.
 *
 *  Portability Issues:
 *      None.
 */

#include <ostream>
#include <sstream>
#include <format>
#include <iomanip>
#include <terra/json/json.h>
#include "unicode_constants.h"

namespace Terra::JSON
{

namespace
{

/*
 *  ConvertToStdString()
 *
 *  Description:
 *      This function will convert a std::u8string to std::string.  The
 *      purpose is to facilitate producing error output.
 *
 *  Parameters:
 *      string [in]
 *          The std::u8string to convert to std::string.
 *
 *  Returns:
 *      The converted std::string.
 *
 *  Comments:
 *      None.
 */
constexpr std::string ConvertToStdString(const std::u8string &string)
{
    return {string.cbegin(), string.cend()};
}

} // namespace

/*
 *  operator<<()
 *
 *  Description:
 *      Streaming operator to produce JSON text for a JSONString type.
 *
 *  Parameters:
 *      o [in]
 *          A reference to the steaming operator onto which the JSON string
 *          will be appended.
 *
 *      string [in]
 *          The string to output as JSON text.
 *
 *  Returns:
 *      A reference to the streaming operator passed in as input.
 *
 *  Comments:
 *      None.
 */
std::ostream &operator<<(std::ostream &o, const JSONString &string)
{
    std::size_t expected_utf8_remaining{};
    std::uint32_t wide_character{};

    // Write out the string start character
    o << '"';

    // Iterate over each character in the string
    for (auto c : string.value)
    {
        // If expecting another UTF-8 character, handle it
        if (expected_utf8_remaining > 0)
        {
            // Look for 10xxxxxx octets
            if ((c & 0xc0) != 0x80)
            {
                throw JSONException(std::string("Invalid UTF-8 character "
                                                "sequence: ") +
                                    ConvertToStdString(string.value));
            }

            // Append additional bits to the wide character
            wide_character = (wide_character << 6) | (c & 0x3f);

            // Decrement the number of expected octets remaining
            expected_utf8_remaining--;

            // If this is the final UTF-8 character, produce the output
            if (expected_utf8_remaining == 0)
            {
                // Verify the character is a valid Unicode value
                if (wide_character > Unicode::Maximum_Character_Value)
                {
                    throw JSONException(std::string("Invalid Unicode "
                                                    "character: ") +
                                        ConvertToStdString(string.value));
                }

                // Ensure the character code is not within the surrogate range
                if ((wide_character >= Unicode::Surrogate_High_Min) &&
                    (wide_character <= Unicode::Surrogate_Low_Max))
                {
                    throw JSONException(std::string("Invalid UTF-8 character "
                                                    "sequence: ") +
                                        ConvertToStdString(string.value));
                }

                // Encode using surrogate code points
                if (wide_character > Unicode::Maximum_BMP_Value)
                {
                    // Convert the code point values using two 16-bit values
                    // (See: https://www.Unicode.org/faq/utf_bom.html#utf16-3)

                    o << std::format(
                        "\\u{:04X}",
                        static_cast<std::uint16_t>(Unicode::Lead_Offset +
                                                   (wide_character >> 10)));

                    o << std::format(
                        "\\u{:04X}",
                        static_cast<std::uint16_t>(Unicode::Surrogate_Low_Min +
                                                   (wide_character & 0x3ff)));
                }
                else
                {
                    // Produce a normal BMP code as \uXXXX
                    o << std::format(
                        "\\u{:04X}",
                        static_cast<std::uint16_t>(wide_character));
                }
            }

            continue;
        }

        // Handle special characters
        switch (c)
        {
            case '"':
                o << "\\\"";
                break;

            case '\\':
                o << "\\\\";
                break;

            case '\b':
                o << "\\b";
                break;

            case '\f':
                o << "\\f";
                break;

            case '\n':
                o << "\\n";
                break;

            case '\r':
                o << "\\r";
                break;

            case '\t':
                o << "\\t";
                break;

            case 0x7e:
                o << "\\u007E";
                break;

            default:
                // Is this a control character?
                if (c < 0x20)
                {
                    o << std::format("\\u{:04X}",
                                     static_cast<std::uint16_t>(c));
                    continue;
                }

                // Does it appear to be a UTF-8 character?
                if (c > 0x7f)
                {
                    // Two octet UTF-8 sequence (110xxxxx)
                    if ((c & 0xe0) == 0xc0)
                    {
                        wide_character = c & 0x3f;
                        expected_utf8_remaining = 1;
                        continue;
                    }

                    // Three octet UTF-8 sequence (1110xxxx)
                    if ((c & 0xf0) == 0xe0)
                    {
                        wide_character = c & 0x0f;
                        expected_utf8_remaining = 2;
                        continue;
                    }

                    // Four octet UTF-8 sequence (11110xxx)
                    if ((c & 0xf8) == 0xf0)
                    {
                        wide_character = c & 0x07;
                        expected_utf8_remaining = 3;
                        continue;
                    }

                    // Any other value would be an invalid character
                    throw JSONException(std::string("Invalid UTF-8 character "
                                                    "sequence: ") +
                                        ConvertToStdString(string.value));
                }

                // Output regular ASCII character
                o << static_cast<char>(c);

                break;
        }
    }

    // If still processing a UTF-8 sequence, that is an error
    if (expected_utf8_remaining > 0)
    {
        throw JSONException(std::string("Invalid UTF-8 character sequence: ") +
                            ConvertToStdString(string.value));
    }

    // Write out the string end character
    o << '"';

    return o;
}

/*
 *  JSONString::ToString()
 *
 *  Description:
 *      This function will convert the JSONString object to a std::string.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A std::string containing JSON text derived from the JSONString object.
 *
 *  Comments:
 *      None.
 */
std::string JSONString::ToString() const
{
    std::ostringstream oss;

    oss << *this;

    return oss.str();
}

} // namespace Terra::JSON
