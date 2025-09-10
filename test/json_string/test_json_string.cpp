/*
 *  test_json_string.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the JSONString object.
 *
 *  Portability Issues:
 *      None.
 */

#include <sstream>
#include <terra/json/json.h>
#include <terra/stf/stf.h>

// Test initialization of JSONString
STF_TEST(JSONString, Constructor1)
{
    Terra::JSON::JSONString string("Test");

    STF_ASSERT_EQ(4, string.Size());
}

// Test initialization of JSONString
STF_TEST(JSONString, Constructor2)
{
    Terra::JSON::JSONString string(u8"Test");

    STF_ASSERT_EQ(4, string.Size());
}

// Test initialization of JSONString
STF_TEST(JSONString, Constructor3)
{
    Terra::JSON::JSONString string(std::string("Test"));

    STF_ASSERT_EQ(4, string.Size());
}

// Test initialization of JSONString
STF_TEST(JSONString, Constructor4)
{
    Terra::JSON::JSONString string(std::u8string(u8"Test"));

    STF_ASSERT_EQ(4, string.Size());
}

// Test initialization of JSONString
STF_TEST(JSONString, Constructor5)
{
    Terra::JSON::JSONString string(std::u8string(u8"Test"));
    Terra::JSON::JSONString string2(string);

    STF_ASSERT_EQ(4, string.Size());
    STF_ASSERT_EQ(4, string2.Size());
}

// Test initialization of JSONString
STF_TEST(JSONString, Constructor6)
{
    Terra::JSON::JSONString string(std::string("Test"));
    Terra::JSON::JSONString string2(string);

    STF_ASSERT_EQ(4, string.Size());
    STF_ASSERT_EQ(4, string2.Size());
}

// Test initialization of JSONString
STF_TEST(JSONString, Assignment1)
{
    Terra::JSON::JSONString string = "Test";

    STF_ASSERT_EQ(4, string.Size());
}

// Test initialization of JSONString
STF_TEST(JSONString, Assignment2)
{
    Terra::JSON::JSONString string = u8"Test";

    STF_ASSERT_EQ(4, string.Size());
}

// Test initialization of JSONString
STF_TEST(JSONString, Assignment3)
{
    Terra::JSON::JSONString string = std::string("Test");

    STF_ASSERT_EQ(4, string.Size());
}

// Test initialization of JSONString
STF_TEST(JSONString, Assignment4)
{
    Terra::JSON::JSONString string = std::u8string(u8"Test");

    STF_ASSERT_EQ(4, string.Size());
}

// Test initialization of JSONString
STF_TEST(JSONString, Assignment5)
{
    Terra::JSON::JSONString string(std::u8string(u8"Test"));
    Terra::JSON::JSONString string2 = string;

    STF_ASSERT_EQ(4, string.Size());
    STF_ASSERT_EQ(4, string2.Size());
}

// Test initialization of JSONString
STF_TEST(JSONString, Assignment6)
{
    Terra::JSON::JSONString string(std::string("Test"));
    Terra::JSON::JSONString string2 = string;

    STF_ASSERT_EQ(4, string.Size());
    STF_ASSERT_EQ(4, string2.Size());
}

// Test JSON text output
STF_TEST(JSONString, Output1)
{
    Terra::JSON::JSONString string(std::string("Test"));
    std::string expected = R"("Test")";

    std::ostringstream oss;

    oss << string;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test JSON text output
STF_TEST(JSONString, Output2)
{
    Terra::JSON::JSONString string(std::string("Test\tString"));
    std::string expected = R"("Test\tString")";

    std::ostringstream oss;

    oss << string;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test JSON text output
STF_TEST(JSONString, Output3)
{
    Terra::JSON::JSONString string(std::string("Test"));
    (*string).push_back(char8_t(0x05));
    *string += u8"String";

    std::string expected = R"("Test\u0005String")";

    std::ostringstream oss;

    oss << string;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test JSON text output
STF_TEST(JSONString, Output4)
{
    Terra::JSON::JSONString string = u8"This string contains ";
    (*string).push_back(0xf0);
    (*string).push_back(0x9f);
    (*string).push_back(0x98);
    (*string).push_back(0x81);
    const std::string expected = R"("This string contains \uD83D\uDE01")";

    std::ostringstream oss;

    oss << string;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test JSON text output
STF_TEST(JSONString, Output5)
{
    Terra::JSON::JSONString string = u8"This string contains ";
    (*string).push_back(0xe5);
    (*string).push_back(0xb0);
    (*string).push_back(0x8f);
    (*string) += u8" character";
    const std::string expected = R"("This string contains \u5C0F character")";

    std::ostringstream oss;

    oss << string;

    STF_ASSERT_EQ(expected, oss.str());
}

// Test the ToString() function
STF_TEST(JSONString, ToString)
{
    Terra::JSON::JSONString string = u8"This string contains ";
    (*string).push_back(0xe5);
    (*string).push_back(0xb0);
    (*string).push_back(0x8f);
    *string += u8" character";
    const std::string expected = R"("This string contains \u5C0F character")";

    std::string result = string.ToString();

    STF_ASSERT_EQ(expected, result);
}

// Test for equality
STF_TEST(JSONString, StringsEqual)
{
    Terra::JSON::JSONString string = u8"Sample string";
    Terra::JSON::JSONString other_string = u8"Sample string";

    STF_ASSERT_EQ(string, other_string);
}

// Test for inequality
STF_TEST(JSONString, StringsUnequal)
{
    Terra::JSON::JSONString string = u8"Sample string";
    Terra::JSON::JSONString other_string = u8"Different string";

    STF_ASSERT_NE(string, other_string);
}
