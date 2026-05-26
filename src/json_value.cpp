/*
 *  json_value.cpp
 *
 *  Copyright (C) 2026
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file contains implementation streaming operators for JSONValue.
 *
 *  Portability Issues:
 *      None.
 */

#include <iostream>
#include <variant>
#include <terra/json/json.h>

namespace
{

// NOLINTBEGIN(misc-multiple-inheritance)
template <class... Ts>
struct overloads : Ts... { using Ts::operator()...; };
// NOLINTEND(misc-multiple-inheritance)

} //namespace

/*
 *  operator<<()
 *
 *  Description:
 *      Streaming operator to produce JSON text for a JSONValue type.
 *
 *  Parameters:
 *      o [in]
 *          A reference to the steaming operator onto which the JSON string
 *          will be appended.
 *
 *      value [in]
 *          The JSONValue to output as JSON text.
 *
 *  Returns:
 *      A reference to the streaming operator passed in as input.
 *
 *  Comments:
 *      None.
 */
std::ostream &operator<<(std::ostream &o, const Terra::JSON::JSONValue &value)
{
    std::visit(overloads{[&o](const auto &value) { o << value; }}, value);

    return o;
}
