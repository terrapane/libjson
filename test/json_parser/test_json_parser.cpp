/*
 *  test_json_parser.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the JSONParser.
 *
 *  Portability Issues:
 *      None.
 */

#include <terra/json/json.h>
#include <terra/json/json_parser.h>
#include <terra/stf/stf.h>

using namespace Terra::JSON;

// Test empty string
STF_TEST(JSONParser, ParseEmptyString)
{
    JSONParser json_parser;

    auto parse = [&]() { json_parser.Parse(""); };

    STF_ASSERT_EXCEPTION_E(parse, JSONException);
}

// Test strings with only whitespace
STF_TEST(JSONParser, ParseWhitespaceString)
{
    JSONParser json_parser;

    {
        auto parse = [&]() { json_parser.Parse(" "); };

        STF_ASSERT_EXCEPTION_E(parse, JSONException);
    }

    {
        auto parse = [&]() { json_parser.Parse(" \n  \n"); };

        STF_ASSERT_EXCEPTION_E(parse, JSONException);
    }

    {
        auto parse = [&]() { json_parser.Parse(" \r\n  \r\n   \n\n  "); };

        STF_ASSERT_EXCEPTION_E(parse, JSONException);
    }
}

// Test parsing of string types
STF_TEST(JSONParser, ParseString1)
{
    JSONParser json_parser;
    std::string json_text = R"(
        "This is a string"
    )";
    const std::u8string expected = u8"This is a string";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::String, result.GetValueType());

    JSONString &actual = std::get<JSONString>(*result);

    STF_ASSERT_EQ(expected, *actual);
}

// Test parsing BMP character
STF_TEST(JSONParser, ParseStringBMPCharacter1)
{
    JSONParser json_parser;
    std::string json_text = R"("This string contains \u5C0F character")";
    std::u8string expected = u8"This string contains ";
    expected.push_back(0xe5);
    expected.push_back(0xb0);
    expected.push_back(0x8f);
    expected += u8" character";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::String, result.GetValueType());

    JSONString &actual = std::get<JSONString>(*result);

    STF_ASSERT_EQ(expected, *actual);
}

// Test parsing BMP character
STF_TEST(JSONParser, ParseStringBMPCharacter2)
{
    JSONParser json_parser;
    std::string json_text = R"("This string contains \u00a9 character")";
    std::u8string expected = u8"This string contains ";
    expected.push_back(0xc2);
    expected.push_back(0xa9);
    expected += u8" character";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::String, result.GetValueType());

    JSONString &actual = std::get<JSONString>(*result);

    STF_ASSERT_EQ(expected, *actual);
}

// Test parsing BMP character
STF_TEST(JSONParser, ParseStringBMPCharacter3)
{
    JSONParser json_parser;
    std::string json_text = R"("This string contains \u0040 character")";
    std::u8string expected = u8"This string contains @ character";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::String, result.GetValueType());

    JSONString &actual = std::get<JSONString>(*result);

    STF_ASSERT_EQ(expected, *actual);
}

// Test parsing Unicode character using surrogates
STF_TEST(JSONParser, ParseStringUnicodeSurrogates)
{
    JSONParser json_parser;
    std::string json_text = R"("This string contains \uD83D\uDE01 character")";
    std::u8string expected = u8"This string contains ";
    expected.push_back(0xf0);
    expected.push_back(0x9f);
    expected.push_back(0x98);
    expected.push_back(0x81);
    expected += u8" character";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::String, result.GetValueType());

    JSONString &actual = std::get<JSONString>(*result);

    STF_ASSERT_EQ(expected, *actual);
}

// Test parsing Unicode character using surrogates (lowercase)
STF_TEST(JSONParser, ParseStringUnicodeSurrogatesLowercase)
{
    JSONParser json_parser;
    std::string json_text = R"("This string contains \ud83d\ude01 character")";
    std::u8string expected = u8"This string contains ";
    expected.push_back(0xf0);
    expected.push_back(0x9f);
    expected.push_back(0x98);
    expected.push_back(0x81);
    expected += u8" character";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::String, result.GetValueType());

    JSONString &actual = std::get<JSONString>(*result);

    STF_ASSERT_EQ(expected, *actual);
}

// Test parsing Unicode character using surrogates at start of string
STF_TEST(JSONParser, ParseStringUnicodeSurrogatesStart)
{
    JSONParser json_parser;
    std::string json_text = R"("\uD83D\uDE01 character")";
    std::u8string expected;
    expected.push_back(0xf0);
    expected.push_back(0x9f);
    expected.push_back(0x98);
    expected.push_back(0x81);
    expected += u8" character";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::String, result.GetValueType());

    JSONString &actual = std::get<JSONString>(*result);

    STF_ASSERT_EQ(expected, *actual);
}

// Test parsing Unicode character using surrogates at end of string
STF_TEST(JSONParser, ParseStringUnicodeSurrogatesEnd)
{
    JSONParser json_parser;
    std::string json_text = R"("This string contains \uD83D\uDE01")";
    std::u8string expected = u8"This string contains ";
    expected.push_back(0xf0);
    expected.push_back(0x9f);
    expected.push_back(0x98);
    expected.push_back(0x81);

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::String, result.GetValueType());

    JSONString &actual = std::get<JSONString>(*result);

    STF_ASSERT_EQ(expected, *actual);
}

// Test parsing of number types
STF_TEST(JSONParser, ParseNumber1)
{
    JSONParser json_parser;
    std::string json_text = R"(
        123
    )";
    const std::int64_t expected = 123;

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Number, result.GetValueType());

    JSONNumber &actual = std::get<JSONNumber>(*result);

    STF_ASSERT_TRUE(std::holds_alternative<JSONInteger>(*actual));

    auto actual_int = std::get<JSONInteger>(*actual);

    STF_ASSERT_EQ(expected, actual_int);
}

// Test parsing of number types
STF_TEST(JSONParser, ParseNumber2)
{
    JSONParser json_parser;
    std::string json_text = R"(
        -345
    )";
    const std::int64_t expected = -345;

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Number, result.GetValueType());

    JSONNumber &actual = std::get<JSONNumber>(*result);

    STF_ASSERT_TRUE(std::holds_alternative<JSONInteger>(*actual));

    auto actual_int = std::get<JSONInteger>(*actual);

    STF_ASSERT_EQ(expected, actual_int);
}

// Test parsing of number types
STF_TEST(JSONParser, ParseNumber3)
{
    JSONParser json_parser;
    std::string json_text = R"(
        2.5
    )";
    const double expected = 2.5;

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Number, result.GetValueType());

    JSONNumber &actual = std::get<JSONNumber>(*result);

    STF_ASSERT_TRUE(std::holds_alternative<JSONFloat>(*actual));

    auto actual_double = std::get<JSONFloat>(*actual);

    STF_ASSERT_CLOSE(expected, actual_double, 0.01);
}

// Test parsing of number types
STF_TEST(JSONParser, ParseNumber4)
{
    JSONParser json_parser;
    std::string json_text = R"(
        1.7e+09
    )";
    const double expected = 1.7e+09;

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Number, result.GetValueType());

    JSONNumber &actual = std::get<JSONNumber>(*result);

    STF_ASSERT_TRUE(std::holds_alternative<JSONFloat>(*actual));

    auto actual_double = std::get<JSONFloat>(*actual);

    STF_ASSERT_CLOSE(expected, actual_double, 0.00001);
}

// Test parsing of number types
STF_TEST(JSONParser, ParseNumber5)
{
    JSONParser json_parser;
    std::string json_text = R"(
        -31.27e+29
    )";
    const double expected = -31.27e+29;

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Number, result.GetValueType());

    JSONNumber &actual = std::get<JSONNumber>(*result);

    STF_ASSERT_TRUE(std::holds_alternative<JSONFloat>(*actual));

    auto actual_double = std::get<JSONFloat>(*actual);

    STF_ASSERT_CLOSE(expected, actual_double, 0.00001);
}

// Test parsing of number types
STF_TEST(JSONParser, ParseNumber6)
{
    JSONParser json_parser;
    std::string json_text = R"(
        31.27e-29
    )";
    const double expected = 31.27e-29;

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Number, result.GetValueType());

    JSONNumber &actual = std::get<JSONNumber>(*result);

    STF_ASSERT_TRUE(std::holds_alternative<JSONFloat>(*actual));

    auto actual_double = std::get<JSONFloat>(*actual);

    STF_ASSERT_CLOSE(expected, actual_double, 0.00001);
}

// Test parsing JSON object
STF_TEST(JSONParser, ParseObject1)
{
    JSONParser json_parser;
    std::string json_text = "{}";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Object, result.GetValueType());

    JSONObject &actual = std::get<JSONObject>(*result);

    // There should be two tag / value pairs
    STF_ASSERT_EQ(0, actual.Size());
}

// Test parsing JSON object
STF_TEST(JSONParser, ParseObject2)
{
    JSONParser json_parser;
    std::string json_text = R"(
        {
            "a": 1,
            "b": 2
        }
    )";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Object, result.GetValueType());

    JSONObject &actual = std::get<JSONObject>(*result);

    // There should be two tag / value pairs
    STF_ASSERT_EQ(2, actual.Size());

    // Verify the values exist
    STF_ASSERT_TRUE(actual.HasKey(u8"a"));
    STF_ASSERT_TRUE(actual.HasKey(u8"b"));
    STF_ASSERT_FALSE(actual.HasKey(u8"c"));

    // Ensure they are the correct value types
    STF_ASSERT_EQ(JSONValueType::Number, actual[u8"a"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, actual[u8"b"].GetValueType());

    // Get the numeric values
    const auto &value_a = std::get<JSONNumber>(*actual[u8"a"]);
    const auto &value_b = std::get<JSONNumber>(*actual[u8"b"]);

    // Check the values are integers
    STF_ASSERT_TRUE(std::holds_alternative<JSONInteger>(*value_a));
    STF_ASSERT_TRUE(std::holds_alternative<JSONInteger>(*value_b));

    // Get the actual integer values
    STF_ASSERT_EQ(1, value_a.GetInteger());
    STF_ASSERT_EQ(2, value_b.GetInteger());
}

// Test parsing JSON object
STF_TEST(JSONParser, ParseObject3)
{
    JSONParser json_parser;
    std::u8string json_text = u8R"(
        {
            "a": {
                "a": 1,
                "b": 2.5,
                "c": 3
            },
            "b": "Some text"
        }
    )";
    const std::u8string expected_b = u8"Some text";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Object, result.GetValueType());

    JSONObject &actual = std::get<JSONObject>(*result);

    // There should be two tag / value pairs
    STF_ASSERT_EQ(2, actual.Size());

    // Verify the values exist
    STF_ASSERT_TRUE(actual.HasKey(u8"a"));
    STF_ASSERT_TRUE(actual.HasKey(u8"b"));
    STF_ASSERT_FALSE(actual.HasKey(u8"c"));

    // Ensure they are the correct value types
    STF_ASSERT_EQ(JSONValueType::Object, actual[u8"a"].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, actual[u8"b"].GetValueType());

    // Get references to the value types for a and b
    auto &value_a = std::get<JSONObject>(*actual[u8"a"]);
    auto &value_b = std::get<JSONString>(*actual[u8"b"]);

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
    STF_ASSERT_EQ(1, std::get<JSONNumber>(*value_a[u8"a"]).GetInteger());
    STF_ASSERT_CLOSE(2.5,
                     std::get<JSONNumber>(*value_a[u8"b"]).GetFloat(),
                     0.0001);
    STF_ASSERT_EQ(3, std::get<JSONNumber>(*value_a[u8"c"]).GetInteger());

    // Verify the value of value_b
    STF_ASSERT_EQ(expected_b, *value_b);
}

// Test parsing JSON array
STF_TEST(JSONParser, ParseArray1)
{
    JSONParser json_parser;
    std::u8string json_text = u8"[]";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Array, result.GetValueType());

    JSONArray &actual = std::get<JSONArray>(*result);

    // There should be two tag / value pairs
    STF_ASSERT_EQ(0, actual.Size());
}

// Test parsing JSON array
STF_TEST(JSONParser, ParseArray2)
{
    JSONParser json_parser;
    std::u8string json_text = u8R"(
        [ 1, "This is a string", true, 2, { "": false } ]
    )";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Array, result.GetValueType());

    JSONArray &actual = std::get<JSONArray>(*result);

    // There should be two tag / value pairs
    STF_ASSERT_EQ(5, actual.Size());

    // Verify the types are correct
    STF_ASSERT_EQ(JSONValueType::Number, actual[0].GetValueType());
    STF_ASSERT_EQ(JSONValueType::String, actual[1].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Literal, actual[2].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Number, actual[3].GetValueType());
    STF_ASSERT_EQ(JSONValueType::Object, actual[4].GetValueType());
}

// Test parsing JSON literal
STF_TEST(JSONParser, ParseLiteral1)
{
    JSONParser json_parser;
    std::string json_text = R"(
        true
    )";
    const JSONLiteral expected = JSONLiteral::True;

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Literal, result.GetValueType());

    JSONLiteral &actual = std::get<JSONLiteral>(*result);

    STF_ASSERT_EQ(expected, actual);
}

// Test parsing JSON literal
STF_TEST(JSONParser, ParseLiteral2)
{
    JSONParser json_parser;
    std::string json_text = R"(
        false
    )";
    const JSONLiteral expected = JSONLiteral::False;

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Literal, result.GetValueType());

    JSONLiteral &actual = std::get<JSONLiteral>(*result);

    STF_ASSERT_EQ(expected, actual);
}

// Test parsing JSON literal
STF_TEST(JSONParser, ParseLiteral3)
{
    JSONParser json_parser;
    std::string json_text = R"(
        null
    )";
    const JSONLiteral expected = JSONLiteral::Null;

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Literal, result.GetValueType());

    JSONLiteral &actual = std::get<JSONLiteral>(*result);

    STF_ASSERT_EQ(expected, actual);
}

// Test parsing JSON literal (failure case)
STF_TEST(JSONParser, ParseLiteral4)
{
    JSONParser json_parser;
    std::string json_text = R"(
        flawed
    )";

    auto parse = [&]() { json_parser.Parse(json_text); };

    STF_ASSERT_EXCEPTION_E(parse, JSONException);
}

// Test parsing JSON literal (failure case)
STF_TEST(JSONParser, ParseLiteral5)
{
    JSONParser json_parser;
    std::string json_text = R"(
        unknown
    )";

    auto parse = [&]() { json_parser.Parse(json_text); };

    STF_ASSERT_EXCEPTION_E(parse, JSONException);
}

// Test copying object
STF_TEST(JSONParser, JSONCopy)
{
    JSONParser json_parser;
    std::u8string json_text = u8R"(
        [ 1, "This is a string", true, 2, { "": false } ]
    )";

    JSON result = json_parser.Parse(json_text);

    STF_ASSERT_EQ(JSONValueType::Array, result.GetValueType());

    // There should be 5 elements in the array
    STF_ASSERT_EQ(5, std::get<JSONArray>(*result).Size());

    // Copy the JSON object
    JSON result_copy = result;

    // There should be 5 elements in the original and duplicate
    STF_ASSERT_EQ(5, std::get<JSONArray>(*result).Size());
    STF_ASSERT_EQ(5, std::get<JSONArray>(*result_copy).Size());
}

// Test moving object
STF_TEST(JSONParser, JSONMove)
{
    JSONParser json_parser;
    std::u8string json_text = u8R"(
        [ 1, "This is a string", true, 2, { "": false } ]
    )";

    JSON result = json_parser.Parse(json_text);

    // Verify the object type is correct
    STF_ASSERT_EQ(JSONValueType::Array, result.GetValueType());

    // There should be 5 elements in the array
    STF_ASSERT_EQ(5, std::get<JSONArray>(*result).Size());

    // Move the JSON object
    JSON result_copy(std::move(result));

    // There should be 5 elements in the array
    STF_ASSERT_EQ(5, std::get<JSONArray>(*result_copy).Size());
}

// Test for equality
STF_TEST(JSONParser, TestEquality)
{
    // These two JSON strings are "equal", but formatted differently and
    // elements are reordered
    const std::string json1 = R"({
    "glossary": {
        "title": "example glossary",
		"GlossDiv": {
            "title": "S",
			"GlossList": {
                "GlossEntry": {
                    "ID": "SGML",
					"Abbrev": "ISO 8879:1986",
					"GlossTerm": "Standard Generalized Markup Language",
					"SortAs": "SGML",
					"Acronym": "SGML",
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
    const std::string json2 = R"({
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

    JSON result1 = JSONParser().Parse(json1);
    JSON result2 = JSONParser().Parse(json2);

    STF_ASSERT_EQ(result1, result2);
}

// Test for inequality
STF_TEST(JSONParser, TestInequality)
{
    // These two JSON strings are almost equal, though formatted is different
    // and elements are reordered -- but array elements reordered in the
    // second instance
    const std::string json1 = R"({
    "glossary": {
        "title": "example glossary",
		"GlossDiv": {
            "title": "S",
			"GlossList": {
                "GlossEntry": {
                    "ID": "SGML",
					"Abbrev": "ISO 8879:1986",
					"GlossTerm": "Standard Generalized Markup Language",
					"SortAs": "SGML",
					"Acronym": "SGML",
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
    const std::string json2 = R"({
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
              "XML",
              "GML"
            ]
          },
          "GlossSee": "markup"
        }
      }
    }
  }
})";

    JSON result1 = JSONParser().Parse(json1);
    JSON result2 = JSONParser().Parse(json2);

    STF_ASSERT_NE(result1, result2);
}

// Test parsing an object with a syntax error
STF_TEST(JSONParser, ParseSyntaxError1)
{
    std::string error_string;
    JSONParser json_parser;

    std::u8string json_text = u8R"(
        [ 1, "This is a string", true 2, { "": false } ]
    )";

    auto parse = [&]() { json_parser.Parse(json_text); };

    try
    {
        parse();
    }
    catch (const JSONException &e)
    {
        error_string = e.what();
    }

    STF_ASSERT_EQ(error_string,
                  std::string("JSON parsing error at line 1, column 38: "
                              "Expected a comma"));
}

// Test parsing an object with a syntax error
STF_TEST(JSONParser, ParseSyntaxError2)
{
    std::string error_string;

    JSONParser json_parser;
    std::u8string json_text = u8R"(
        [ 1, "This is a string", true, { x } ]
    )";

    auto parse = [&]() { json_parser.Parse(json_text); };

    try
    {
        parse();
    }
    catch (const JSONException &e)
    {
        error_string = e.what();
    }

    STF_ASSERT_EQ(error_string,
                  std::string("JSON parsing error at line 1, column 41: "
                              "Unknown value type"));
}

// Test parsing an object with a syntax error
STF_TEST(JSONParser, ParseSyntaxError3)
{
    std::string error_string;

    JSONParser json_parser;
    std::u8string json_text = u8R"(
        [ 1, "This
        is a string", true, { x } ]
    )";

    auto parse = [&]() { json_parser.Parse(json_text); };

    try
    {
        parse();
    }
    catch (const JSONException &e)
    {
        error_string = e.what();
    }

    STF_ASSERT_EQ(error_string,
                  std::string("JSON parsing error at line 1, column 18: "
                              "Illegal control character in string"));
}
