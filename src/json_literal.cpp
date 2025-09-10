/*
 *  json_literal.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file contains implementation of some functions defined for the
 *      JSONLiteral type.
 *
 *  Portability Issues:
 *      None.
 */

#include <terra/json/json.h>

/*
 *  operator<<()
 *
 *  Description:
 *      Streaming operator to produce JSON text for a JSONLiteral type.
 *
 *  Parameters:
 *      o [in]
 *          A reference to the steaming operator onto which the JSON string
 *          will be appended.
 *
 *      literal [in]
 *          The JSONLiteral value to output as JSON text.
 *
 *  Returns:
 *      A reference to the streaming operator passed in as input.
 *
 *  Comments:
 *      None.
 */
std::ostream &operator<<(std::ostream &o,
                         const Terra::JSON::JSONLiteral literal)
{
    switch (literal)
    {
        case Terra::JSON::JSONLiteral::True:
            o << "true";
            break;

        case Terra::JSON::JSONLiteral::False:
            o << "false";
            break;

        case Terra::JSON::JSONLiteral::Null:
            o << "null";
            break;

        default:
            throw Terra::JSON::JSONException("Invalid JSON Literal value");
    }

    return o;
}
