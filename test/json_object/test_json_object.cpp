/*
 *  test_json_object.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the JSONObject object.
 *
 *  Portability Issues:
 *      None.
 */

#include <terra/json/json.h>
#include <terra/stf/stf.h>

using namespace Terra::JSON;

// Test basic construction
STF_TEST(JSONObject, Construction1)
{
    JSONObject object1({{"key", "Value"}});
    JSONObject object2 = {{"key", "Value"}};

    STF_ASSERT_EQ(1, object1.Size());
    STF_ASSERT_EQ(1, object2.Size());
}

// Test initializer list
STF_TEST(JSONObject, InitializerList1)
{
    JSONObject object =
    {
        {u8"Key1", "Value"},
        {u8"Key2", 25},
        {u8"Key3", JSONLiteral::True},
        {u8"Key4", {{1, 2, 3}}},
        {u8"Key5", {
                        {
                            {u8"Key1", "foo"},
                            {u8"Key2", "bar"}
                        }
                   }
        }
    };

    // Verify there are five elements in the map
    STF_ASSERT_EQ(5, object.value.size());
    STF_ASSERT_EQ(5, object.Size());
}

// Test initializer list
STF_TEST(JSONObject, InitializerList2)
{
    JSONString some_string = "Test";
    JSONString some_string2 = u8"Test";
    JSONObject object =
    {
        {"Key1", "Value"},
        {"Key2", 25},
        {"Key3", some_string},
        {"Key4", some_string2},
        {"Key5", JSONObject(
                        {
                            {u8"Key1", "foo"},
                            {u8"Key2", "bar"}
                        })},
        {"Key6", "Hello"},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key10", JSONLiteral::Null},
        {"Key11", {{1, 2}}}
    };

    // Verify the number of elements
    STF_ASSERT_EQ(11, object.value.size());
    STF_ASSERT_EQ(11, object.Size());

    // Verify the types
    STF_ASSERT_EQ(JSONValueType::String, object["Key1"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, object["Key2"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["Key3"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["Key4"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Object, object["Key5"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["Key6"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["Key7"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, object["Key8"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, object["Key9"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Literal, object["Key10"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Array, object["Key11"].GetValueType());
}

// Test basic assignment
STF_TEST(JSONObject, BasicAssignment)
{
    JSONObject object;

    object["Key1"] = 10;
    object["Key2"] = "Some string";
    object["Key3"] = {1, 2, 3};

    // Verify there are three elements in the map
    STF_ASSERT_EQ(3, object.value.size());
    STF_ASSERT_EQ(3, object.Size());

    // Verify the last item is an array with three elements
    STF_ASSERT_EQ(JSONValueType::Array, object["Key3"].GetValueType());
    STF_ASSERT_EQ(3, object["Key3"].GetValue<JSONArray>().Size());
}

// Test assignment operator
STF_TEST(JSONObject, IndexOperator1)
{
    JSONObject object;

    // Assign values
    object["key1"] = 10;
    object["key2"] = 20;
    object["key3"] = 30;
    object["key4"] = u8"Hello";
    object["key5"] = std::string("Hello");
    object["key6"] = "Hello";
    object["key7"] = std::string("Hello");
    object["key8"] = 3.14;

    // Verify there are seven elements in the map
    STF_ASSERT_EQ(8, object.value.size());
    STF_ASSERT_EQ(8, object.Size());

    // Verify that the type are assigned as expected
    STF_ASSERT_EQ(JSONValueType::Number, object["key1"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, object["key3"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, object["key2"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["key4"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["key5"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["key6"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["key7"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, object["key8"].GetValueType());

    // Verify the the numbers are recognized as the right type
    STF_ASSERT_TRUE(object["key2"].GetValue<JSONNumber>().IsInteger());
    STF_ASSERT_TRUE(object["key8"].GetValue<JSONNumber>().IsFloat());
}

// Test reading JSONObjects using assignment operator
STF_TEST(JSONObject, IndexOperator2)
{
    JSONObject object;

    // Assign values
    object["key1"] = 10;
    object["key2"] = 20;
    object["key3"] = 30;

    // Verify there are three elements in the map
    STF_ASSERT_EQ(3, object.value.size());
    STF_ASSERT_EQ(3, object.Size());

    // Now attempt to read
    JSON json_number = object["key2"];

    // This should be a number
    STF_ASSERT_EQ(JSONValueType::Number, json_number.GetValueType());

    // Verify the value is correct
    STF_ASSERT_EQ(20, json_number.GetValue<JSONNumber>().GetInteger());
}

// Test streaming operator
STF_TEST(JSONObject, Output1)
{
    JSONObject object;

    object["Key"] = "Some string";

    std::string expected = R"({"Key": "Some string"})";

    std::ostringstream oss;

    oss << object;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming operator
STF_TEST(JSONObject, Output2)
{
    JSONObject object;

    object["Key"] = {1, 2};

    std::string expected = R"({"Key": [1, 2]})";

    std::ostringstream oss;

    oss << object;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test streaming operator
STF_TEST(JSONObject, Output3)
{
    JSONString some_string = "Test";
    JSONString some_string2 = u8"Test";
    JSONObject object =
    {
        {"Key1", "Value"},
        {"Key2", 25},
        {"Key3", some_string},
        {"Key4", some_string2},
        {"Key5", {
                     {
                         {u8"Key1", "foo"},
                         {u8"Key2", "bar"}
                     }
                 }
        },
        {"Key6", "Hello"},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key10", JSONLiteral::Null},
        {"Key11", {{1, 2}}}
    };

    std::ostringstream oss;

    oss << object;

    // Parse the json string produced
    JSON json = JSONParser().Parse(oss.str());

    // Verify that we have a JSON object
    STF_ASSERT_EQ(JSONValueType::Object, json.GetValueType());

    // Re-assign object to be that of the JSON value
    object = json.GetValue<JSONObject>();

    // Verify the number of elements
    STF_ASSERT_EQ(11, object.value.size());
    STF_ASSERT_EQ(11, object.Size());

    // Verify the types
    STF_ASSERT_EQ(JSONValueType::String, object["Key1"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, object["Key2"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["Key3"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["Key4"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Object, object["Key5"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["Key6"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, object["Key7"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, object["Key8"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, object["Key9"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Literal, object["Key10"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Array, object["Key11"].GetValueType());
}

// Test the ToString() function
STF_TEST(JSONObject, ToString)
{
    JSONObject object;

    object["Key"] = {1, 2};

    std::string expected = R"({"Key": [1, 2]})";

    std::string result = object.ToString();

    STF_ASSERT_EQ(expected, result);
}

