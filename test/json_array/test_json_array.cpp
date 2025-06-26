/*
 *  test_json_array.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the JSONArray object.
 *
 *  Portability Issues:
 *      None.
 */

#include <terra/json/json.h>
#include <terra/stf/stf.h>

using namespace Terra::JSON;

// Test initialization via assignment
STF_TEST(JSONArray, InitializerList1)
{
    // This will initialize the array of JSON objects holding JSONNumber types
    JSONArray array({1, 2, 3});

    // Verify there are three elements in the array
    STF_ASSERT_EQ(3, array.value.size());
    STF_ASSERT_EQ(3, array.Size());
}

// Test initialization via the constructor
STF_TEST(JSONArray, InitializerList2)
{
    // This will initialize the array of JSON objects holding JSONNumber types
    JSONArray array({1, 2, 3});

    // Verify there are three elements in the array
    STF_ASSERT_EQ(3, array.value.size());
    STF_ASSERT_EQ(3, array.Size());
}

// Test the indexing operator
STF_TEST(JSONArray, IndexOperator1)
{
    // This will initialize the array of JSON objects holding JSONNumber types
    JSONArray array({1, 2, 3});

    // This should return a JSON object holding a number
    JSON item = array[1];

    // Verify that it is a number
    STF_ASSERT_EQ(JSONValueType::Number, item.GetValueType());

    // Verify the number is the expected value
    STF_ASSERT_EQ(2, item.GetValue<JSONNumber>().GetInteger());
}

// Test the indexing operator
STF_TEST(JSONArray, IndexOperator2)
{
    // This will initialize the array of JSON objects holding JSONNumber types
    JSONArray array({1, 2, 3});

    // Assign array position 1 with the value 12
    array[1] = 12;

    // Verify that it is a number
    STF_ASSERT_EQ(JSONValueType::Number, array[1].GetValueType());

    // Verify the number is the expected value
    STF_ASSERT_EQ(12, array[1].GetValue<JSONNumber>().GetInteger());
}

// Test streaming output operator
STF_TEST(JSONArray, Output1)
{
    // This will initialize the array of JSON objects holding JSONNumber types
    JSONArray array({1, 2, 3});

    std::string expected = R"([1, 2, 3])";

    std::ostringstream oss;

    oss << array;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming output operator (empty array)
STF_TEST(JSONArray, Output2)
{
    // This will initialize the array of JSON objects holding JSONNumber types
    JSONArray array;

    std::string expected = R"([])";

    std::ostringstream oss;

    oss << array;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test the ToString() function
STF_TEST(JSONArray, ToString)
{
    // This will initialize the array of JSON objects holding JSONNumber types
    JSONArray array({1, 2, 3});

    std::string expected = R"([1, 2, 3])";

    std::string result = array.ToString();

    STF_ASSERT_EQ(expected, result);
}

// Test equality
STF_TEST(JSONArray, TestEquality)
{
    // This will initialize the array of JSON objects holding JSONNumber types
    JSONArray array1({1, {{{"Key1", "Value1"}, {"Key2", "Value2"}}}, 2});
    JSONArray array2({1, {{{"Key2", "Value2"}, {"Key1", "Value1"}}}, 2});

    STF_ASSERT_EQ(array1, array2);
}

// Test equality
STF_TEST(JSONArray, TestInequality)
{
    // This will initialize the array of JSON objects holding JSONNumber types
    JSONArray array1({1, {{{"Key1", "Value1"}, {"Key2", "Value2"}}}, 2});
    JSONArray array2({2, {{{"Key2", "Value2"}, {"Key1", "Value1"}}}, 1});

    STF_ASSERT_NE(array1, array2);
}
