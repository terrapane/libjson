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
 *      The JSONParser will parse and deserializes JSON text and form a JSON
 *      object.  This object is not intended to be used by several threads at
 *      once, but this is a relatively light-weight object that can be
 *      instantiated, Parse() called, and destroyed as needed.  This could be
 *      a simple function, but encapsulating the state within the object makes
 *      the interface cleaner.
 *
 *      If there is an error parsing JSON text, a JSONException exception will
 *      be thrown.
 *
 *  Portability Issues:
 *      Requires C++20 or later.
 */

#include <cstddef>
#include <string>
#include <algorithm>
#include <iterator>
#include "json.h"

#pragma once

namespace Terra::JSON
{

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

        constexpr bool EndOfInput() const noexcept { return p == q; }
        constexpr std::size_t RemainingInput() const noexcept
        {
            return static_cast<std::size_t>(std::distance(p, q));
        }
        void AdvanceReadPosition(std::size_t steps = 1) noexcept
        {
            auto advance = std::min(steps, RemainingInput());
            p += advance;
            column += advance;
        }
        void ConsumeWhitespace() noexcept;
        JSONValueType DetermineValueType() const;
        JSONValue ParseContent();
        JSONValue ParsePrimitiveValue(JSONValueType value_type);
        void ParseCompositeValue();
        JSONString ParseString();
        void ParseUnicode(JSONString &json_string);
        JSONNumber ParseNumber();
        void ParseObject();
        void ParseArray();
        JSONLiteral ParseLiteral();

        std::u8string_view::const_iterator p;   // Input iterator (start)
        std::u8string_view::const_iterator q;   // Input iterator (end)
        std::size_t line;                       // Current line number
        std::size_t column;                     // Current column

        // Used to parse composite types
        std::vector<CompositeContext> composite_context;
};

} // namespace Terra::JSON
