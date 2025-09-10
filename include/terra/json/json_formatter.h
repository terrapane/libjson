/*
 *  json_formatter.h
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the JSONFormatter, which will accept JSON
 *      text as input and produce formatted JSON output.  By default, the
 *      JSON output will use two space indentions, but this may be configured
 *      during object construction.
 *
 *      The JSONFormatter accepts a JSON text string and reformats it using the
 *      specified amount of indentation.  It will essentially parse the text,
 *      but lacks some of the more rigid parsing logic in the JSONParser.
 *      (In short, do not rely on the JSONFormatter to enforce proper syntax,
 *      but nonetheless do expect an exception if a syntax error is detected.)
 *
 *  Portability Issues:
 *      Requires C++20 or later.
 */

#pragma once

#include "json.h"

namespace Terra::JSON
{

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
        void Print(std::ostream &stream, const std::string_view content);
        void Print(std::ostream &stream, const std::u8string_view content);

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
