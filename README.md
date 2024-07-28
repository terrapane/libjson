# JSON Library

## Introduction

This library implements a JSON parser and serializer, allowing JSON data to be
stored and accessed easily in C++.  One may serialize the JSON object using
`ToString()` or streaming operators (`<<`).  By default, the serialization
logic produces a compact encoding on a single line.  There is also a
`JSONFormatter` object that can be used to produce formatted output with
vertical and configurable horizon whitespace.

Using the JSON library is pretty simple.  The important thing to remember
is that JSON input text might contain errors, so the parser will throw an
exception when parsing.  The `JSONFormatter` also throws an exception if
the input text contains certain types of errors.

Another thing to be mindful about is that JSON is comprised of UTF-8 strings.
Thus, this library uses `std::u8string` strings.  When adding object
keys, it is the user's responsibility to ensure the key values are
properly encoded UTF-8 text.  One may also use `std::string` types as input
when parsing, but the assumption is still that those are UTF-8 strings.

## JSON Object

The main object one should use is called `JSON`.  It holds a `std::variant`
type called `JSONValue` that holds one of several types of JSON
values, namely:

```text
JSONLiteral - Represents a JSON literal type (e.g., null)
JSONNumber - An integer or floating point value
JSONString - A JSON string (this MUST be in UTF-8!)
JSONArray - An array of JSON types
JSONObject - A map of string/JSON type pairs
```

## Parsing JSON text

To parse JSON text, one can do something like this:

```cpp
std::u8string json_text = u8R"(
    [
        1,
        "This is a string",
        true,
        2,
        { "key1": false, "key2": "test" }
    ]
)";

JSON json = JSONParser().Parse(json_text);
```

In this example, there is an array containing four elements.  Each element
of a JSON array might contain a different type.  In this example there are
numeric values, literals, a string, and an object.

When parsing random JSON data, one might not know the type of data. The easiest
way to find out is to call `json.GetValueType()`.  This will return a type
of `JSONValueType`, which is an enumeration type that will indicate the
type of data.  In this case, it would be `JSONValueType::Array`.

### Accessing data

Knowing the type of data, accessing it using the `[]` operator.  For example,
to get the integer value in array position 3, one can do this:

```cpp
std::int64_t number = json[3].GetValue<JSONNumber>().GetInteger();
```

Note the use of `GetValue<T>()` here.  This is always required when accessing
a `JSON` object since the type of data held might be one of the aforementioned
types.

The functions `GetValue<T>()` and `[]` return a reference, allowing one to
access members more easily.  Consider array position 4 in the example in
the previous section.  This is also a JSON object.  To make it easier to
manipulate, one may do this:

```cpp
// Get a reference to the object in array position 4
JSONObject &json_object = json[4].GetValue<JSONObject>();

// Get the literal value at "key1"
JSONLiteral literal = json_object["key1"].GetValue<JSONLiteral>();

// Assign the literal value
json_object["key1"] = JSONLiteral::Null;

// Get the string value at "key2"
std::u8string string = json_object["key2"].GetValue<JSONString>().value;
```

Note the `value` member in that last example.  For most types (`JSONObject`,
`JSONNumber`, `JSONString`, and `JSONArray`) there is a `value` member to allow
direct access to the data. It's not required, but it is a public member to give
the user flexibility.

In the example above, `GetValue<JSONString>` is used to return a `JSONString`
reference.  If the key value was not a string, this would cause an exception
to be thrown.  Thus, it is important to check the type of value before
accessing it.

While accessing some of the data appears verbose, data structures are
generally known and checks can be skipped.  Checking each type is generally
only important when the structure is not fully known.  However, one should
wrap operations in `try`/`catch` blocks.

### Assigning data

To assign the integer value in position 3 (again from the example above), one
may do the following:

```cpp
json[3] = 4;
```

Assignment is much simpler, but that's because one does not need to check the
type of data to make an assignment.  This is a way to assign a string in
array position 4 having the key "key2":

```cpp
try
{
    json[4]["key2"] = u8"New string value";
}
catch (const JSONException &e)
{
    // handle exception
}
```

Handling the potential for an exception is important, as one will be thrown
if a request to access data is invalid.  While normally creating a `JSON`
object will not throw an exception, the example above poses a risk since there
is an assumption that `json` is an array with at least a valid element 4
that holds a `JSONObject`.

Initializers are defined to make it easy to assign JSON objects naturally.
The following show some example assignments.

```cpp
// Simple literal assignment
JSON literal = JSONLiteral::Null;

// Simple number assignment
JSON number = -3e-05;

// Single string assignment
JSON string = "Test string";

// Simple array assignment
JSON array = {{1, 2, 3}};

// Assign a JSON object with string values, object values, and and array
JSON object = {
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
```

## JSON numbers

JSON allows numbers to be either floating point or integer values.  This
library will make that distinction, though how important that is to your
application may vary.  To make it easy, there are member functions for
the `JSONNumber` type to return a numeric value of the desired type, regardless
of what is stored.  They are `GetInteger()` and `GetFloat()`.  There are also
function to tell you what type of number is stored called `IsFloat()` and
`IsInteger()`.  JSON does not dictate the size of integer or floating point
values, though many web browsers impose limits.  This library will not
enforce a limit, but just uses the `JSONInteger` and `JSONFloat` types defined
to store those values.  Note that all integers are assumed to be signed.
Thus, it's not possible to store a large unsigned integer that would overflow
the JSONInteger type.  It is the user's responsibility to not use an integer
that is too large.

When parsing JSON, the JSONParser will automatically construct a `JSONInteger`
or `JSONFloat` based on whether the number appears to be a floating point value
or an integer.  Since this is just a "best guess" (based on the presence of
a decimal point or `e` in the numeric value), it would be reasonable to
just use the `GetFloat()` function, for example, without checking the
type (since this will not throw an exception) if you know the number should
be treated as a floating point, for example.

## Serializing a JSON object

It is quite easy to serialize a JSON object.  It can be done like this:

```cpp
std::ostringstream oss;

// Assuming object is a JSON object type
oss << object;
```

There are also functions for most objects called `ToString()` that will
produce the same string output.

The format of the default serialization is for all output to be on a single
line with one space between values.  If one would like to have vertical
whitespace and variable horizontal spacing, one may use the `JSONFormatter`
object like this:

```cpp
// Here, "json" may be a JSON object or json text, as Print is an overloaded
// function that will accept either
std::string json_string = JSONFormatter().Print(json);
```

Note that serializing data may also result in an exception if, as examples,
strings are not stored as valid UTF-8 or numeric values are illegal.
As an example of an illegal number, `inf` (infinite) is not a valid floating
point value per the JSON specification.
