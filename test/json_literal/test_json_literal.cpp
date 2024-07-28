/*
 *  test_json_literal.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the JSONLiteral-related operations.
 *
 *  Portability Issues:
 *      None.
 */

#include <sstream>
#include <terra/json/json.h>
#include <terra/stf/stf.h>

using namespace Terra::JSON;

// Test streaming operator
STF_TEST(JSONLiteral, Output1)
{
    JSONLiteral literal = JSONLiteral::True;
    std::string expected = "true";

    std::ostringstream oss;

    oss << literal;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming operator
STF_TEST(JSONLiteral, Output2)
{
    JSONLiteral literal = JSONLiteral::False;
    std::string expected = "false";

    std::ostringstream oss;

    oss << literal;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming operator
STF_TEST(JSONLiteral, Output3)
{
    JSONLiteral literal = JSONLiteral::Null;
    std::string expected = "null";

    std::ostringstream oss;

    oss << literal;

    STF_ASSERT_EQ(expected, oss.str());
}
