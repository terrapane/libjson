/*
 *  test_json_number.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the JSONNumber object.
 *
 *  Portability Issues:
 *      None.
 */

#include <sstream>
#include <terra/json/json.h>
#include <terra/stf/stf.h>

using namespace Terra::JSON;

// Test initialization of a JSONNumber object
STF_TEST(JSONNumber, Constructor1)
{
    JSONNumber number(1);

    STF_ASSERT_FALSE(number.IsFloat());
    STF_ASSERT_TRUE(number.IsInteger());

    STF_ASSERT_EQ(1, number.GetInteger());
}

// Test initialization of a JSONNumber object
STF_TEST(JSONNumber, Constructor2)
{
    JSONNumber number(1.0);

    STF_ASSERT_TRUE(number.IsFloat());
    STF_ASSERT_FALSE(number.IsInteger());

    STF_ASSERT_CLOSE(1.0, number.GetFloat(), 0.0001);
}

// Test initialization of a JSONNumber object
STF_TEST(JSONNumber, Assignment1)
{
    JSONNumber number = 1;

    STF_ASSERT_FALSE(number.IsFloat());
    STF_ASSERT_TRUE(number.IsInteger());

    STF_ASSERT_EQ(1, number.GetInteger());
}

// Test initialization of a JSONNumber object
STF_TEST(JSONNumber, Assignment2)
{
    JSONNumber number = 1.0;

    STF_ASSERT_TRUE(number.IsFloat());
    STF_ASSERT_FALSE(number.IsInteger());

    STF_ASSERT_CLOSE(1.0, number.GetFloat(), 0.0001);
}

// Test initialization of a JSONNumber object
STF_TEST(JSONNumber, Assignment3)
{
    JSONNumber number = 1;

    number = 2;

    STF_ASSERT_FALSE(number.IsFloat());
    STF_ASSERT_TRUE(number.IsInteger());

    STF_ASSERT_EQ(2, number.GetInteger());
}

// Test initialization of a JSONNumber object
STF_TEST(JSONNumber, Assignment4)
{
    JSONNumber number = 1.0;

    number = 2.5;

    STF_ASSERT_TRUE(number.IsFloat());
    STF_ASSERT_FALSE(number.IsInteger());

    STF_ASSERT_CLOSE(2.5, number.GetFloat(), 0.0001);
}

// Test streaming operator for positive integer
STF_TEST(JSONNumber, Output1)
{
    JSONNumber number(12345);
    std::string expected = R"(12345)";

    std::ostringstream oss;

    oss << number;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming operator for negative integer
STF_TEST(JSONNumber, Output2)
{
    JSONNumber number(-34);
    std::string expected = R"(-34)";

    std::ostringstream oss;

    oss << number;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming operator for floating point
STF_TEST(JSONNumber, Output3)
{
    JSONNumber number(1.5);
    std::string expected = R"(1.5)";

    std::ostringstream oss;

    oss << number;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming operator for very small floating point
STF_TEST(JSONNumber, Output4)
{
    JSONNumber number(-0.0000000001234);
    std::string expected = R"(-1.234e-10)";

    std::ostringstream oss;

    oss << number;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming operator for negative zero
STF_TEST(JSONNumber, Output5)
{
    JSONNumber number(-0.0);
    std::string expected = R"(0)";

    std::ostringstream oss;

    oss << number;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming operator for zero
STF_TEST(JSONNumber, Output6)
{
    JSONNumber number(0.0);
    std::string expected = R"(0)";

    std::ostringstream oss;

    oss << number;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test the ToString() function
STF_TEST(JSONNumber, ToString)
{
    JSONNumber number(-34);
    std::string expected = R"(-34)";

    std::string result = number.ToString();

    STF_ASSERT_EQ(expected, result);
}

// Test for equality
STF_TEST(JSONNumber, NumberEqual)
{
    JSONNumber number(1.5);
    JSONNumber other_number(1.5);

    STF_ASSERT_EQ(number, other_number);
}

// Test for inequality
STF_TEST(JSONNumber, NumberUnequal)
{
    JSONNumber number(1.8);
    JSONNumber other_number(1.5);
    JSONNumber yet_another_number(2);

    STF_ASSERT_NE(number, other_number);
    STF_ASSERT_NE(number, yet_another_number);
}

// Test re-assignment
STF_TEST(JSONNumber, ReAssignment1)
{
    JSONNumber number = 1;

    STF_ASSERT_TRUE(number.IsInteger());
    STF_ASSERT_EQ(1, number.GetInteger());

    number = 5.3;

    STF_ASSERT_TRUE(number.IsFloat());
    STF_ASSERT_EQ(5.3, number.GetFloat());
}

// Test re-assignment
STF_TEST(JSONNumber, ReAssignment2)
{
    const double new_value = 2.3;

    JSONNumber number = 1;

    STF_ASSERT_TRUE(number.IsInteger());
    STF_ASSERT_EQ(1, number.GetInteger());

    number = new_value;

    STF_ASSERT_TRUE(number.IsFloat());
    STF_ASSERT_EQ(new_value, number.GetFloat());
}
