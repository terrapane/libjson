/*
 *  test_json_literal.cpp
 *
 *  Copyright (C) 2024, 2025, 2026
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
#include <string>
#include <terra/json/json.h>
#include <terra/stf/stf.h>

namespace
{

using namespace Terra::JSON;

// Test streaming operator
STF_TEST(JSONLiteral, Output1)
{
    const JSONLiteral literal = JSONLiteral::True;
    const std::string expected = "true";

    std::ostringstream oss;

    oss << literal;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming operator
STF_TEST(JSONLiteral, Output2)
{
    const JSONLiteral literal = JSONLiteral::False;
    const std::string expected = "false";

    std::ostringstream oss;

    oss << literal;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming operator
STF_TEST(JSONLiteral, Output3)
{
    const JSONLiteral literal = JSONLiteral::Null;
    const std::string expected = "null";

    std::ostringstream oss;

    oss << literal;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test for equality
STF_TEST(JSONLiteral, TestEquality)
{
    const JSONLiteral literal1 = JSONLiteral::Null;
    const JSONLiteral literal2 = JSONLiteral::Null;

    STF_ASSERT_EQ(literal1, literal2);
}

// Test for inequality
STF_TEST(JSONLiteral, TestInequality)
{
    const JSONLiteral literal1 = JSONLiteral::Null;
    const JSONLiteral literal2 = JSONLiteral::True;

    STF_ASSERT_NE(literal1, literal2);
}

} // namespace
