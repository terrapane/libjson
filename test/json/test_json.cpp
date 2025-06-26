/*
 *  test_json.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the JSON object.
 *
 *  Portability Issues:
 *      None.
 */

#include <terra/json/json.h>
#include <terra/stf/stf.h>

using namespace Terra::JSON;

// Test construction / assignment with JSON object
STF_TEST(JSON, ConstructJSON1)
{
    JSON json = JSONParser().Parse(R"(
        {
            "a": {
                "a": 1,
                "b": 2.5,
                "c": 3
            },
            "b": "Some text",
            "c": null
        }
    )");

    const std::u8string expected_b = u8"Some text";

    STF_ASSERT_EQ(JSONValueType::Object, json.GetValueType());

    JSONObject &actual = json.GetValue<JSONObject>();

    // There should be two tag / value pairs
    STF_ASSERT_EQ(3, actual.Size());

    // Verify the values exist
    STF_ASSERT_TRUE(actual.HasKey(u8"a"));
    STF_ASSERT_TRUE(actual.HasKey(u8"b"));
    STF_ASSERT_TRUE(actual.HasKey(u8"c"));
    STF_ASSERT_FALSE(actual.HasKey(u8"d"));

    // Ensure they are the correct value types
    STF_ASSERT_EQ(JSONValueType::Object, actual[u8"a"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, actual[u8"b"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Literal, actual[u8"c"].GetValueType());

    // Get references to the value types for a and b
    auto &value_a = actual[u8"a"].GetValue<JSONObject>();
    auto &value_b = actual[u8"b"].GetValue<JSONString>();
    auto value_c = actual[u8"c"].GetValue<JSONLiteral>();

    // Now check the contents of value_a
    STF_ASSERT_EQ(3, value_a.Size());

    // Verify the values of value_a exist
    STF_ASSERT_TRUE(value_a.HasKey(u8"a"));
    STF_ASSERT_TRUE(value_a.HasKey(u8"b"));
    STF_ASSERT_TRUE(value_a.HasKey(u8"c"));

    // Ensure they are the correct value types
    STF_ASSERT_EQ(JSONValueType::Number, value_a[u8"a"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, value_a[u8"b"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, value_a[u8"c"].GetValueType());

    // Verify the numeric values of value_a
    STF_ASSERT_EQ(1, value_a[u8"a"].GetValue<JSONNumber>().GetInteger());
    STF_ASSERT_CLOSE(2.5,
                     value_a[u8"b"].GetValue<JSONNumber>().GetFloat(),
                     0.0001);
    STF_ASSERT_EQ(3, value_a[u8"c"].GetValue<JSONNumber>().GetInteger());

    // Verify the value of value_b
    STF_ASSERT_EQ(expected_b, *value_b);

    // Verify the expected value of value_c
    STF_ASSERT_EQ(JSONLiteral::Null, value_c);
}

// Test construction / assignment with JSON object
STF_TEST(JSON, ConstructJSON2)
{
    JSON json = JSONParser().Parse("null");

    STF_ASSERT_EQ(JSONValueType::Literal, json.GetValueType());

    JSONLiteral actual = json.GetValue<JSONLiteral>();

    // There should be two tag / value pairs
    STF_ASSERT_EQ(JSONLiteral::Null, actual);

}

// Test construction
STF_TEST(JSON, ConstructJSON3)
{
    JSON json(u8"Hello, World!");
    const std::u8string expected = u8"Hello, World!";

    STF_ASSERT_EQ(JSONValueType::String, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json.GetValue<JSONString>());
}

// Test construction
STF_TEST(JSON, ConstructJSON4)
{
    JSON json;
    const std::u8string expected = u8"Hello, Again!";

    json = "Hello, Again!";

    STF_ASSERT_EQ(JSONValueType::String, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json.GetValue<JSONString>());
}

// Test construction
STF_TEST(JSON, ConstructJSON5)
{
    JSON json = u8"Hello, World!";
    const std::u8string expected = u8"Hello, World!";

    STF_ASSERT_EQ(JSONValueType::String, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json.GetValue<JSONString>());
}

// Test construction
STF_TEST(JSON, ConstructJSON6)
{
    JSON json = "Hello, Again!";
    const std::u8string expected = u8"Hello, Again!";

    STF_ASSERT_EQ(JSONValueType::String, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json.GetValue<JSONString>());
}

// Test construction
STF_TEST(JSON, ConstructJSON7)
{
    JSON json = 12345;
    const std::int64_t expected = 12345;

    // Verify the JSON type is a number
    STF_ASSERT_EQ(JSONValueType::Number, json.GetValueType());

    // Verify the it is not a float
    STF_ASSERT_FALSE(json.GetValue<JSONNumber>().IsFloat());

    // Verify the string value
    STF_ASSERT_EQ(expected, json.GetValue<JSONNumber>().GetInteger());
}

// Test construction
STF_TEST(JSON, ConstructJSON8)
{
    JSON json = 3.14159;
    const double expected = 3.14159;

    // Verify the JSON type is a number
    STF_ASSERT_EQ(JSONValueType::Number, json.GetValueType());

    // Verify the it is a float
    STF_ASSERT_TRUE(json.GetValue<JSONNumber>().IsFloat());

    // Verify the string value
    STF_ASSERT_CLOSE(expected, json.GetValue<JSONNumber>().GetFloat(), 0.001);
}

// Test construction
STF_TEST(JSON, ConstructJSON9)
{
    JSON json(JSONLiteral::True);
    const JSONLiteral expected = JSONLiteral::True;

    // Verify the JSON type is a number
    STF_ASSERT_EQ(JSONValueType::Literal, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, json.GetValue<JSONLiteral>());
}

// Test construction
STF_TEST(JSON, ConstructJSON10)
{
    JSON json = JSONLiteral::True;
    const JSONLiteral expected = JSONLiteral::True;

    // Verify the JSON type is a number
    STF_ASSERT_EQ(JSONValueType::Literal, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, json.GetValue<JSONLiteral>());
}

// Test string assignment
STF_TEST(JSON, StringAssignment1)
{
    JSON json;
    const std::u8string expected = u8"Hello, World!";

    json = u8"Hello, World!";

    STF_ASSERT_EQ(JSONValueType::String, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json.GetValue<JSONString>());
}

// Test string assignment
STF_TEST(JSON, StringAssignment2)
{
    JSON json;
    const std::u8string expected = u8"Hello, Again!";

    json = "Hello, Again!";

    STF_ASSERT_EQ(JSONValueType::String, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json.GetValue<JSONString>());
}

// Test number assignment
STF_TEST(JSON, NumberAssignment1)
{
    JSON json;
    const std::int64_t expected = 12345;

    json = expected;

    // Verify the JSON type is a number
    STF_ASSERT_EQ(JSONValueType::Number, json.GetValueType());

    // Verify the it is not a float
    STF_ASSERT_FALSE(json.GetValue<JSONNumber>().IsFloat());

    // Verify the string value
    STF_ASSERT_EQ(expected, json.GetValue<JSONNumber>().GetInteger());
}

// Test number assignment
STF_TEST(JSON, NumberAssignment2)
{
    JSON json;
    const double expected = 3.14159;

    json = expected;

    // Verify the JSON type is a number
    STF_ASSERT_EQ(JSONValueType::Number, json.GetValueType());

    // Verify the it is a float
    STF_ASSERT_TRUE(json.GetValue<JSONNumber>().IsFloat());

    // Verify the string value
    STF_ASSERT_EQ(expected, json.GetValue<JSONNumber>().GetFloat());
}

// Test number assignment
STF_TEST(JSON, JSONLiteralAssignment)
{
    JSON json;
    const JSONLiteral expected = JSONLiteral::True;

    json = expected;

    // Verify the JSON type is a number
    STF_ASSERT_EQ(JSONValueType::Literal, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, json.GetValue<JSONLiteral>());
}

// Test JSON assignment
STF_TEST(JSON, JSONAssignment1)
{
    JSON json;
    const std::u8string expected = u8"Hello, World!";

    json = u8"Hello, World!";

    // Verify the right type
    STF_ASSERT_EQ(JSONValueType::String, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json.GetValue<JSONString>());

    // Create a new object and assign it
    JSON json2;
    json2 = json;

    // Verify the right type
    STF_ASSERT_EQ(JSONValueType::String, json2.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json2.GetValue<JSONString>());
}

// Test JSON assignment
STF_TEST(JSON, JSONAssignment2)
{
    JSON json;
    const std::u8string expected = u8"Hello, World!";

    json = u8"Hello, World!";

    // Verify the right type
    STF_ASSERT_EQ(JSONValueType::String, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json.GetValue<JSONString>());

    // Create a new object and assign it
    JSON json2(json);

    // Verify the right type
    STF_ASSERT_EQ(JSONValueType::String, json2.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json2.GetValue<JSONString>());
}

// Test JSON assignment
STF_TEST(JSON, JSONAssignment3)
{
    JSON json;
    JSON json2;
    const std::u8string expected = u8"Hello, World!";

    json2 = json = u8"Hello, World!";

    // Verify the right type
    STF_ASSERT_EQ(JSONValueType::String, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json.GetValue<JSONString>());

    // Verify the right type
    STF_ASSERT_EQ(JSONValueType::String, json2.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json2.GetValue<JSONString>());
}

// Test JSON assignment
STF_TEST(JSON, JSONMove1)
{
    JSON json;
    const std::u8string expected = u8"Hello, World!";

    json = u8"Hello, World!";

    // Verify the right type
    STF_ASSERT_EQ(JSONValueType::String, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json.GetValue<JSONString>());

    // Create a new object and assign it
    JSON json2 = std::move(json);

    // Verify the right type
    STF_ASSERT_EQ(JSONValueType::String, json2.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json2.GetValue<JSONString>());
}

// Test JSON assignment
STF_TEST(JSON, JSONMove2)
{
    JSON json;
    const std::u8string expected = u8"Hello, World!";

    json = u8"Hello, World!";

    // Verify the right type
    STF_ASSERT_EQ(JSONValueType::String, json.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json.GetValue<JSONString>());

    // Create a new object and assign it
    JSON json2(std::move(json));

    // Verify the right type
    STF_ASSERT_EQ(JSONValueType::String, json2.GetValueType());

    // Verify the string value
    STF_ASSERT_EQ(expected, *json2.GetValue<JSONString>());
}

// Test initialization via assignment
STF_TEST(JSON, InitializerList1)
{
    // This will initialize the array of JSON objects holding JSONNumber types
    JSON json = JSONArray{JSONNumber(1), JSONNumber(2), JSONNumber(3)};

    // Ensure there is an array
    STF_ASSERT_EQ(JSONValueType::Array, json.GetValueType());

    // Verify there are three elements in the array
    STF_ASSERT_EQ(3, json.GetValue<JSONArray>().Size());
}

// Test initialization via the constructor
STF_TEST(JSON, InitializerList2)
{
    // This will initialize the array of JSON objects holding JSONNumber types
    JSON json({JSONNumber(1), JSONNumber(2), JSONNumber(3)});

    // Ensure there is an array
    STF_ASSERT_EQ(JSONValueType::Array, json.GetValueType());

    // Verify there are three elements in the array
    STF_ASSERT_EQ(3, json.GetValue<JSONArray>().Size());
}

// Test initialization via assignment
STF_TEST(JSON, InitializerList3)
{
    JSON json = JSONObject{
        {"Key1", JSONString("Value")},
        {"Key2", JSONNumber(25)},
        {"Key3", JSONLiteral::True},
        {"Key4", JSONArray({JSONNumber(1), JSONNumber(2), JSONNumber(3)})},
        {"Key5", JSONObject(
                        {
                            {"Key1", JSONString("foo")},
                            {"Key2", JSONString("bar")}
                        })}};

    // Ensure the JSON object holds the expected type
    STF_ASSERT_EQ(JSONValueType::Object, json.GetValueType());

    // Verify there are five elements in the map
    STF_ASSERT_EQ(5, json.GetValue<JSONObject>().value.size());
    STF_ASSERT_EQ(5, json.GetValue<JSONObject>().Size());
}

// Test initialization via assignment
STF_TEST(JSON, InitializerList4)
{
    JSON json(
    {
        {"Key1", JSONString("Value")},
        {"Key2", JSONNumber(25)},
        {"Key3", JSONLiteral::True},
        {"Key4", JSONArray({JSONNumber(1), JSONNumber(2), JSONNumber(3)})},
        {"Key5", JSONObject(
                        {
                            {"Key1", JSONString("foo")},
                            {"Key2", JSONString("bar")}
                        })}
    });

    // Ensure the JSON object holds the expected type
    STF_ASSERT_EQ(JSONValueType::Object, json.GetValueType());

    // Verify there are five elements in the map
    STF_ASSERT_EQ(5, json.GetValue<JSONObject>().value.size());
    STF_ASSERT_EQ(5, json.GetValue<JSONObject>().Size());
}

// Test initialization via assignment
STF_TEST(JSON, InitializerList5)
{
    JSON json = {
    {
        {"Key1", "String Value"},
        {"Key2", 25},
        {"Key3", "some string"},
        {"Key4", "sone other string"},
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
        {"Key11", {{1, 2, 3}}}
    }};

    // Ensure the JSON object holds the expected type
    STF_ASSERT_EQ(JSONValueType::Object, json.GetValueType());

    // Get a reference to the JSONObject type
    auto &object = json.GetValue<JSONObject>();

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

// Test the indexing operator
STF_TEST(JSON, AccessOperator1)
{
    // This will initialize the JSON with an array of JSON objects holding
    // JSONNumber types
    JSON json({JSONNumber(1), JSONNumber(2), JSONNumber(3)});

    // Verify the JSON object holds an array
    STF_ASSERT_EQ(JSONValueType::Array, json.GetValueType());

    // This should return a JSON object holding a number
    JSON item = json[1];

    // Verify that it is a number
    STF_ASSERT_EQ(JSONValueType::Number, item.GetValueType());

    // Verify the number is the expected value
    STF_ASSERT_EQ(2, item.GetValue<JSONNumber>().GetInteger());
}

// Test the indexing operator
STF_TEST(JSON, AccessOperator2)
{
    // This will initialize the JSON with an array of JSON objects holding
    // JSONNumber types
    JSON json({JSONNumber(1), JSONNumber(2), JSONNumber(3)});

    // Verify the JSON object holds an array
    STF_ASSERT_EQ(JSONValueType::Array, json.GetValueType());

    // This should return a JSON object holding a number
    json[1] = JSONNumber(12);

    // Verify that it is a number
    STF_ASSERT_EQ(JSONValueType::Number, json[1].GetValueType());

    // Verify the number is the expected value
    STF_ASSERT_EQ(12, json[1].GetValue<JSONNumber>().GetInteger());
}

// Test index operator
STF_TEST(JSON, AccessOperator3)
{
    JSON object;

    // Verify the JSON object holds an object
    STF_ASSERT_EQ(JSONValueType::Object, object.GetValueType());

    // Assign values
    object["key1"] = JSONNumber(10);
    object["key2"] = JSONNumber(20);
    object["key3"] = JSONNumber(30);

    // Verify there are three elements in the array
    STF_ASSERT_EQ(3, object.GetValue<JSONObject>().value.size());
    STF_ASSERT_EQ(3, object.GetValue<JSONObject>().Size());
}

// Test reading JSONObjects using index operator
STF_TEST(JSON, AccessOperator4)
{
    JSON object;

    // Verify the JSON object holds an object
    STF_ASSERT_EQ(JSONValueType::Object, object.GetValueType());

    // Assign values
    object["key1"] = JSONNumber(10);
    object["key2"] = JSONNumber(20);
    object["key3"] = JSONNumber(30);

    // Verify there are three elements in the array
    STF_ASSERT_EQ(3, object.GetValue<JSONObject>().value.size());
    STF_ASSERT_EQ(3, object.GetValue<JSONObject>().Size());

    // Now attempt to read
    JSON json_number = object["key2"];

    // This should be a number
    STF_ASSERT_EQ(JSONValueType::Number, json_number.GetValueType());

    // Verify the value is correct
    STF_ASSERT_EQ(20, json_number.GetValue<JSONNumber>().GetInteger());
}

// Test assigning JSON arrays
STF_TEST(JSON, JSONArrayAssignment)
{
    JSON object;

    // Ensure the object is a JSONObject type
    STF_ASSERT_EQ(JSONValueType::Object, object.GetValueType());

    JSONArray array({1, 2, 3});

    object = array;

    // Ensure the object now contains a JSONArray type
    STF_ASSERT_EQ(JSONValueType::Array, object.GetValueType());

    // Verify the element count
    STF_ASSERT_EQ(3, array.Size());
    STF_ASSERT_EQ(3, object.GetValue<JSONArray>().Size());
}

// Test streaming operator
STF_TEST(JSON, StreamingOperator)
{
    JSONString some_string = "Test";
    JSONString some_string2 = u8"Test";
    JSON json = JSONObject{
    {
        {"Key1", JSONString("Value")},
        {"Key2", JSONNumber(25)},
        {"Key3", some_string},
        {"Key4", some_string2},
        {"Key5", JSONObject(
                        {
                            {u8"Key1", JSONString("foo")},
                            {u8"Key2", JSONString("bar")}
                        })},
        {"Key6", "Hello"},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key10", JSONLiteral::Null},
        {"Key11", JSONArray({JSONNumber(1), JSONNumber(2)})}
    }};

    std::ostringstream oss;

    // Produce the output
    oss << json;

    // Parse the json string produced
    json = JSONParser().Parse(oss.str());

    // Verify that we have a JSON object
    STF_ASSERT_EQ(JSONValueType::Object, json.GetValueType());

    // Assign object to be that of the JSON value
    JSONObject object = json.GetValue<JSONObject>();

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

// Test the ToString function
STF_TEST(JSON, ToString)
{
    JSONString some_string = "Test";
    JSONString some_string2 = u8"Test";
    JSON json = JSONObject{
    {
        {"Key1", JSONString("Value")},
        {"Key2", JSONNumber(25)},
        {"Key3", some_string},
        {"Key4", some_string2},
        {"Key5", JSONObject(
                        {
                            {u8"Key1", JSONString("foo")},
                            {u8"Key2", JSONString("bar")}
                        })},
        {"Key6", "Hello"},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key10", JSONLiteral::Null},
        {"Key11", JSONArray({JSONNumber(1), JSONNumber(2)})}
    }};

    std::ostringstream oss;

    // Produce the output
    std::string json_string = json.ToString();

    // Parse the json string produced
    json = JSONParser().Parse(json_string);

    // Verify that we have a JSON object
    STF_ASSERT_EQ(JSONValueType::Object, json.GetValueType());

    // Assign object to be that of the JSON value
    JSONObject object = json.GetValue<JSONObject>();

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

// Test for equality
STF_TEST(JSON, TestEquality)
{
    JSONString some_string = "Test";
    JSONString some_string2 = u8"Test";
    JSON json1 = JSONObject{
    {
        {"Key1", JSONString("Value")},
        {"Key2", JSONNumber(25)},
        {"Key3", some_string},
        {"Key4", some_string2},
        {"Key5", JSONObject(
                        {
                            {u8"Key1", JSONString("foo")},
                            {u8"Key2", JSONString("bar")}
                        })},
        {"Key6", "Hello"},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key10", JSONLiteral::Null},
        {"Key11", JSONArray({JSONNumber(1), JSONNumber(2)})}
    }};

    // Same data as above, but just re-ordered
    JSON json2 = JSONObject{
    {
        {"Key2", JSONNumber(25)},
        {"Key1", JSONString("Value")},
        {"Key4", some_string2},
        {"Key10", JSONLiteral::Null},
        {"Key5", JSONObject(
                        {
                            {u8"Key2", JSONString("bar")},
                            {u8"Key1", JSONString("foo")}
                        })},
        {"Key6", "Hello"},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key3", some_string},
        {"Key11", JSONArray({JSONNumber(1), JSONNumber(2)})}
    }};

    STF_ASSERT_EQ(json1, json2);
}

// Test for inequality
STF_TEST(JSON, TestInequality)
{
    JSONString some_string = "Test";
    JSONString some_string2 = u8"Test";
    JSON json1 = JSONObject{
    {
        {"Key1", JSONString("Value")},
        {"Key2", JSONNumber(25)},
        {"Key3", some_string},
        {"Key4", some_string2},
        {"Key5", JSONObject(
                        {
                            {u8"Key1", JSONString("foo")},
                            {u8"Key2", JSONString("bar")}
                        })},
        {"Key6", "Hello"},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key10", JSONLiteral::Null},
        {"Key11", JSONArray({JSONNumber(1), JSONNumber(2)})}
    }};

    // Same data as above, but changed slightly
    JSON json2 = JSONObject{
    {
        {"Key1", JSONString("Value")},
        {"Key2", JSONNumber(25)},
        {"Key3", some_string},
        {"Key4", some_string2},
        {"Key5", JSONObject(
                        {
                            {u8"Key1", JSONString("foo")},
                            {u8"Key2", JSONString("bar")}
                        })},
        {"Key6", "Hello"},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key10", JSONLiteral::True},
        {"Key11", JSONArray({JSONNumber(1), JSONNumber(2)})}
    }};

    STF_ASSERT_NE(json1, json2);
}
