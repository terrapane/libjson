/*
 *  test_json_formatter.cpp
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the JSONFormatter.  Since Windows and Linux
 *      will use CRLF or LF (respectively), this test code will remove
 *      CR characters from both the expected strings and the produced
 *      output for comparison.  That makes each test a little more verbose.
 *
 *  Portability Issues:
 *      None.
 */

#include <algorithm>
#include <terra/json/json.h>
#include <terra/stf/stf.h>

using namespace Terra::JSON;

STF_TEST(JSONFormatter, JSONLiteral)
{
    std::string expected;
    std::string result;
    JSON json = JSONLiteral::False;
    const std::string expected_raw = R"(false)";

    // Produce the expected string having no \r characters
    std::copy_if(expected_raw.begin(),
                 expected_raw.end(),
                 std::back_inserter(expected),
                 [](char c) { return c != '\r'; });

    std::ostringstream oss;

    // Convert to the JSON object to a string (default formatting)
    oss << json;

    // Produce a formatted string
    std::string formatted_string = JSONFormatter().Print(oss.str());

    // Produce the result string without \r characters
    std::copy_if(formatted_string.begin(),
                 formatted_string.end(),
                 std::back_inserter(result),
                 [](char c) { return c != '\r'; });

    STF_ASSERT_EQ(expected, result);
}

STF_TEST(JSONFormatter, JSONNumber)
{
    std::string expected;
    std::string result;
    JSON json = -2.5;
    const std::string expected_raw = R"(-2.5)";

    // Produce the expected string having no \r characters
    std::copy_if(expected_raw.begin(),
                 expected_raw.end(),
                 std::back_inserter(expected),
                 [](char c) { return c != '\r'; });

    std::ostringstream oss;

    // Convert to the JSON object to a string (default formatting)
    oss << json;

    // Produce a formatted string
    std::string formatted_string = JSONFormatter().Print(oss.str());

    // Produce the result string without \r characters
    std::copy_if(formatted_string.begin(),
                 formatted_string.end(),
                 std::back_inserter(result),
                 [](char c) { return c != '\r'; });

    STF_ASSERT_EQ(expected, result);
}

STF_TEST(JSONFormatter, JSONString)
{
    std::string expected;
    std::string result;
    JSON json = "some string";
    const std::string expected_raw = R"("some string")";

    // Produce the expected string having no \r characters
    std::copy_if(expected_raw.begin(),
                 expected_raw.end(),
                 std::back_inserter(expected),
                 [](char c) { return c != '\r'; });

    std::ostringstream oss;

    // Convert to the JSON object to a string (default formatting)
    oss << json;

    // Produce a formatted string
    std::string formatted_string = JSONFormatter().Print(oss.str());

    // Produce the result string without \r characters
    std::copy_if(formatted_string.begin(),
                 formatted_string.end(),
                 std::back_inserter(result),
                 [](char c) { return c != '\r'; });

    STF_ASSERT_EQ(expected, result);
}

// Test a JSON string in some predetermined order
STF_TEST(JSONFormatter, JSONObject1)
{
    std::string expected;
    std::string result;
    JSONString some_string = "Test";
    JSONString some_string2 = u8"Test";
    std::string json = R"({"Key11": [1, 2], "Key10": null, "Key9": 10, "Key6": "Hello", "Key8": 5.3, "Key5": {"Key2": "bar", "Key1": "foo"}, "Key4": "Test", "Key3": "Test", "Key2": 25, "Key7": "Hello", "Key1": "Value"})";
    const std::string expected_raw = R"({
    "Key11": [
        1,
        2
    ],
    "Key10": null,
    "Key9": 10,
    "Key6": "Hello",
    "Key8": 5.3,
    "Key5": {
        "Key2": "bar",
        "Key1": "foo"
    },
    "Key4": "Test",
    "Key3": "Test",
    "Key2": 25,
    "Key7": "Hello",
    "Key1": "Value"
})";

    // Produce the expected string having no \r characters
    std::copy_if(expected_raw.begin(),
                 expected_raw.end(),
                 std::back_inserter(expected),
                 [](char c) { return c != '\r'; });

    // Produce a formatted string (NOTE: Uses 4 space indentions)
    std::string formatted_string = JSONFormatter(4).Print(json);

    // Produce the result string without \r characters
    std::copy_if(formatted_string.begin(),
                 formatted_string.end(),
                 std::back_inserter(result),
                 [](char c) { return c != '\r'; });

    STF_ASSERT_EQ(expected, result);
}

// Test a JSON object whose order is dictated by the JSON parer (alphabetic)
STF_TEST(JSONFormatter, JSONObject2)
{
    std::string expected;
    std::string result;
    JSONString some_string = "Test";
    JSONString some_string2 = u8"Test";
    JSON json = JSONObject{
    {
        {"Key2", JSONNumber(25)},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key11", JSONArray({JSONNumber(1), JSONNumber(2)})},
        {"Key3", some_string},
        {"Key1", JSONString("Value")},
        {"Key6", "Hello"},
        {"Key10", JSONLiteral::Null},
        {"Key4", some_string2},
        {"Key5", JSONObject(
                        {
                            {u8"Key2", JSONString("bar")},
                            {u8"Key1", JSONString("foo")}
                        })}
    }};
    const std::string expected_raw = R"({
    "Key1": "Value",
    "Key10": null,
    "Key11": [
        1,
        2
    ],
    "Key2": 25,
    "Key3": "Test",
    "Key4": "Test",
    "Key5": {
        "Key1": "foo",
        "Key2": "bar"
    },
    "Key6": "Hello",
    "Key7": "Hello",
    "Key8": 5.3,
    "Key9": 10
})";

    // Produce the expected string having no \r characters
    std::copy_if(expected_raw.begin(),
                 expected_raw.end(),
                 std::back_inserter(expected),
                 [](char c) { return c != '\r'; });

    std::ostringstream oss;

    // Convert to the JSON object to a string (default formatting)
    oss << json;

    // Produce a formatted string (NOTE: Uses 4 space indentions)
    std::string formatted_string = JSONFormatter(4).Print(oss.str());

    // Produce the result string without \r characters
    std::copy_if(formatted_string.begin(),
                 formatted_string.end(),
                 std::back_inserter(result),
                 [](char c) { return c != '\r'; });

    STF_ASSERT_EQ(expected, result);
}

STF_TEST(JSONFormatter, JSONArray1)
{
    std::string expected;
    std::string result;
    JSONArray json({JSONNumber(1), JSONNumber(2), JSONNumber(3)});
    const std::string expected_raw = R"([
  1,
  2,
  3
])";

    // Produce the expected string having no \r characters
    std::copy_if(expected_raw.begin(),
                 expected_raw.end(),
                 std::back_inserter(expected),
                 [](char c) { return c != '\r'; });

    std::ostringstream oss;

    // Convert to the JSON object to a string (default formatting)
    oss << json;

    // Produce a formatted string
    std::string formatted_string = JSONFormatter().Print(oss.str());

    // Produce the result string without \r characters
    std::copy_if(formatted_string.begin(),
                 formatted_string.end(),
                 std::back_inserter(result),
                 [](char c) { return c != '\r'; });

    STF_ASSERT_EQ(expected, result);
}

STF_TEST(JSONFormatter, JSONArray2)
{
    std::string expected;
    std::string result;
    JSONString some_string = "Test";
    JSONString some_string2 = u8"Test";
    JSONObject json_object{
    {
        {"Key2", JSONNumber(25)},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key11", JSONArray({JSONNumber(1), JSONNumber(2)})},
        {"Key3", some_string},
        {"Key1", JSONString("Value")},
        {"Key6", "Hello"},
        {"Key10", JSONLiteral::Null},
        {"Key4", some_string2},
        {"Key5", JSONObject(
                        {
                            {u8"Key2", JSONString("bar")},
                            {u8"Key1", JSONString("foo")}
                        })}
    }};
    JSONArray json({JSONNumber(1), json_object, JSONNumber(3)});
    const std::string expected_raw = R"([
  1,
  {
    "Key1": "Value",
    "Key10": null,
    "Key11": [
      1,
      2
    ],
    "Key2": 25,
    "Key3": "Test",
    "Key4": "Test",
    "Key5": {
      "Key1": "foo",
      "Key2": "bar"
    },
    "Key6": "Hello",
    "Key7": "Hello",
    "Key8": 5.3,
    "Key9": 10
  },
  3
])";

    // Produce the expected string having no \r characters
    std::copy_if(expected_raw.begin(),
                 expected_raw.end(),
                 std::back_inserter(expected),
                 [](char c) { return c != '\r'; });

    std::ostringstream oss;

    // Convert to the JSON object to a string (default formatting)
    oss << json;

    // Produce a formatted string
    std::string formatted_string = JSONFormatter().Print(oss.str());

    // Produce the result string without \r characters
    std::copy_if(formatted_string.begin(),
                 formatted_string.end(),
                 std::back_inserter(result),
                 [](char c) { return c != '\r'; });

    STF_ASSERT_EQ(expected, result);
}

// Same test as above, but passing the json object to Print()
STF_TEST(JSONFormatter, JSONArray3)
{
    std::string expected;
    std::string result;
    JSONString some_string = "Test";
    JSONString some_string2 = u8"Test";
    JSONObject json_object{
    {
        {"Key2", JSONNumber(25)},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key11", JSONArray({JSONNumber(1), JSONNumber(2)})},
        {"Key3", some_string},
        {"Key1", JSONString("Value")},
        {"Key6", "Hello"},
        {"Key10", JSONLiteral::Null},
        {"Key4", some_string2},
        {"Key5", JSONObject(
                        {
                            {u8"Key2", JSONString("bar")},
                            {u8"Key1", JSONString("foo")}
                        })}
    }};
    JSON json({JSONNumber(1), json_object, JSONNumber(3)});
    const std::string expected_raw = R"([
  1,
  {
    "Key1": "Value",
    "Key10": null,
    "Key11": [
      1,
      2
    ],
    "Key2": 25,
    "Key3": "Test",
    "Key4": "Test",
    "Key5": {
      "Key1": "foo",
      "Key2": "bar"
    },
    "Key6": "Hello",
    "Key7": "Hello",
    "Key8": 5.3,
    "Key9": 10
  },
  3
])";

    // Produce the expected string having no \r characters
    std::copy_if(expected_raw.begin(),
                 expected_raw.end(),
                 std::back_inserter(expected),
                 [](char c) { return c != '\r'; });

    // Produce a formatted string (passing the JSON object directly)
    std::string formatted_string = JSONFormatter().Print(json);

    // Produce the result string without \r characters
    std::copy_if(formatted_string.begin(),
                 formatted_string.end(),
                 std::back_inserter(result),
                 [](char c) { return c != '\r'; });

    STF_ASSERT_EQ(expected, result);
}

// Same test as above, but using Allman style
STF_TEST(JSONFormatter, JSONArray4)
{
    std::string expected;
    std::string result;
    JSONString some_string = "Test";
    JSONString some_string2 = u8"Test";
    JSONObject json_object{
    {
        {"Key2", JSONNumber(25)},
        {"Key7", u8"Hello"},
        {"Key8", 5.3},
        {"Key9", 10},
        {"Key11", JSONArray({JSONNumber(1), JSONNumber(2)})},
        {"Key3", some_string},
        {"Key1", JSONString("Value")},
        {"Key6", "Hello"},
        {"Key10", JSONLiteral::Null},
        {"Key4", some_string2},
        {"Key5", JSONObject(
                        {
                            {u8"Key2", JSONString("bar")},
                            {u8"Key1", JSONString("foo")}
                        })}
    }};
    JSON json({JSONNumber(1), json_object, JSONNumber(3)});
    const std::string expected_raw = R"([
  1,
  {
    "Key1": "Value",
    "Key10": null,
    "Key11":
    [
      1,
      2
    ],
    "Key2": 25,
    "Key3": "Test",
    "Key4": "Test",
    "Key5":
    {
      "Key1": "foo",
      "Key2": "bar"
    },
    "Key6": "Hello",
    "Key7": "Hello",
    "Key8": 5.3,
    "Key9": 10
  },
  3
])";

    // Produce the expected string having no \r characters
    std::copy_if(expected_raw.begin(),
                 expected_raw.end(),
                 std::back_inserter(expected),
                 [](char c) { return c != '\r'; });

    // Produce a formatted string (passing the JSON object directly)
    std::string formatted_string = JSONFormatter(2, true).Print(json);

    // Produce the result string without \r characters
    std::copy_if(formatted_string.begin(),
                 formatted_string.end(),
                 std::back_inserter(result),
                 [](char c) { return c != '\r'; });

    STF_ASSERT_EQ(expected, result);
}

// This test uses the sample JSON file on json.org
STF_TEST(JSONFormatter, SampleJSON)
{
    const std::string sample_json = R"({
    "glossary": {
        "title": "example glossary",
		"GlossDiv": {
            "title": "S",
			"GlossList": {
                "GlossEntry": {
                    "ID": "SGML",
					"SortAs": "SGML",
					"GlossTerm": "Standard Generalized Markup Language",
					"Acronym": "SGML",
					"Abbrev": "ISO 8879:1986",
					"GlossDef": {
                        "para": "A meta-markup language, used to create markup languages such as DocBook.",
						"GlossSeeAlso": ["GML", "XML"]
                    },
					"GlossSee": "markup"
                }
            }
        }
    }
})";
    const std::string expected = R"({
  "glossary": {
    "title": "example glossary",
    "GlossDiv": {
      "title": "S",
      "GlossList": {
        "GlossEntry": {
          "ID": "SGML",
          "SortAs": "SGML",
          "GlossTerm": "Standard Generalized Markup Language",
          "Acronym": "SGML",
          "Abbrev": "ISO 8879:1986",
          "GlossDef": {
            "para": "A meta-markup language, used to create markup languages such as DocBook.",
            "GlossSeeAlso": [
              "GML",
              "XML"
            ]
          },
          "GlossSee": "markup"
        }
      }
    }
  }
})";

    std::string formatted_string = JSONFormatter().Print(sample_json);

    STF_ASSERT_EQ(expected, formatted_string);
}
