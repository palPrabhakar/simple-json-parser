#include "parser.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace sjp;

Json parseJSON(std::istringstream json) {
    Parser parser(json);
    return parser.Parse();
}

Json parseJSON(std::string json_str) {
    std::istringstream json(json_str);
    Parser parser(json);
    return parser.Parse();
}

// Valid Json
TEST(JsonParserTest, SimpleObject) {
    std::istringstream json(R"({"key": "value"})");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("key").value().Get<std::string>(), "value");
}

TEST(JsonParserTest, NestedObject) {
    std::istringstream json(R"({"user": {"name": "Alice", "age": 30}})");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("user").value().Get("name").value().Get<std::string>(),
              "Alice");
    EXPECT_EQ(result.Get("user").value().Get("age").value().Get<double>(), 30);
}

TEST(JsonParserTest, Array) {
    std::istringstream json(R"({"numbers": [1, 2, 3, 4]})");
    auto result = parseJSON(std::move(json));
    // EXPECT_EQ(result.Get("numbers").size(), 4);
    EXPECT_EQ(result.Get("numbers").value().Get(0).value().Get<double>(), 1);
    EXPECT_EQ(result.Get("numbers").value().Get(1).value().Get<double>(), 2);
}

TEST(JsonParserTest, MixedTypes) {
    std::istringstream json(
        R"({"nullValue": null, "boolean": true, "number": 42})");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("nullValue").value().Get<JNull>(), std::nullopt);
    EXPECT_EQ(result.Get("boolean").value().Get<bool>(), true);
    EXPECT_EQ(result.Get("number").value().Get<double>(), 42);
}

TEST(JsonParserTest, SpecialCharacters) {
    std::istringstream json(R"({"text": "Line1\nLine2\tIndented"})");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("text").value().Get<std::string>().value(),
              R"(Line1\nLine2\tIndented)");
}

// Invalid Json
TEST(JsonParserTest, MissingBrace) {
    std::istringstream json(R"({"key": "value")"); // Missing closing brace
    EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
}

TEST(JsonParserTest, InvalidKeyFormat) {
    std::istringstream json(R"({key: "value"})"); // Key not in quotes
    EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
}

TEST(JsonParserTest, UnexpectedToken) {
    std::istringstream json(R"({"key": "value", "extra"})"); // Invalid
    EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
}

TEST(JsonParserTest, InvalidNumber) {
    std::istringstream json(R"({"number": 1e9999})"); // Out-of-range number
    EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
}

TEST(JsonParserTest, UnterminatedString) {
    std::istringstream json(
        R"({"key": "value})"); // Missing closing quote for string
    EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
}

// edge cases
TEST(JsonParserTest, EmptyObject) {
    std::istringstream json(R"({})");
    EXPECT_NO_THROW(parseJSON(std::move(json)));
}

TEST(JsonParserTest, OnlyWhitespace) {
    std::istringstream json("   ");
    EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
}

TEST(JsonParserTest, InvalidDataType) {
    std::istringstream json(R"({"key": invalid})");
    EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
}

TEST(JsonParserTest, DuplicateKeys) {
    std::istringstream json(R"({"key": "value1", "key": "value2"})");
    EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
}

TEST(JsonParserTest, EmptyArray) {
    std::istringstream json(R"({"array": []})");
    EXPECT_NO_THROW(parseJSON(std::move(json)));
}

TEST(JsonParserTest, TrailingComma) {
    std::istringstream json(R"({"key1": "value1", "key2": "value2",})");
    EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
}

// complex cases
TEST(JsonParserTest, DeeplyNestedObject) {
    std::istringstream json(R"({
        "level1": {
            "level2": {
                "level3": {
                    "level4": {
                        "level5": {"key": "value"}
                    }
                }
            }
        }
    })");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("level1")
                  .value()
                  .Get("level2")
                  .value()
                  .Get("level3")
                  .value()
                  .Get("level4")
                  .value()
                  .Get("level5")
                  .value()
                  .Get("key")
                  .value()
                  .Get<std::string>(),
              "value");
}

TEST(JsonParserTest, LargeNestedArray) {
    std::istringstream json(R"({"data": [[[[[[[1]]]]]]]})");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("data")
                  .value()
                  .Get(0)
                  .value()
                  .Get(0)
                  .value()
                  .Get(0)
                  .value()
                  .Get(0)
                  .value()
                  .Get(0)
                  .value()
                  .Get(0)
                  .value()
                  .Get(0)
                  .value()
                  .Get<double>(),
              1);
}

TEST(JsonParserTest, MixedArrayAndObjects) {
    std::istringstream json(R"({
        "users": [
            {"id": 1, "name": "Alice", "active": true},
            {"id": 2, "name": "Bob", "active": false},
            {"id": 3, "name": "Charlie", "active": true}
        ]
    })");
    auto result = parseJSON(std::move(json));
    // EXPECT_EQ(result.Get("users").size(), 3);
    EXPECT_EQ(result.Get("users")
                  .value()
                  .Get(0)
                  .value()
                  .Get("name")
                  .value()
                  .Get<std::string>(),
              "Alice");
    EXPECT_EQ(result.Get("users")
                  .value()
                  .Get(1)
                  .value()
                  .Get("active")
                  .value()
                  .Get<bool>(),
              false);
    EXPECT_EQ(result.Get("users")
                  .value()
                  .Get(2)
                  .value()
                  .Get("id")
                  .value()
                  .Get<double>(),
              3);
}

TEST(JsonParserTest, LargeJSONFile) {
    std::string json(R"({"data": [)");
    for (int i = 0; i < 100000; ++i) {
        json += std::to_string(i) + ",";
    }
    json.back() = ']'; // Replace the last comma with a closing bracket
    json += "}";
    auto result = parseJSON(std::istringstream(json));
    // EXPECT_EQ(result.Get("data").size(), 100000);
    EXPECT_EQ(result.Get("data").value().Get(0).value().Get<double>(), 0);
    EXPECT_EQ(result.Get("data").value().Get(99999).value().Get<double>(),
              99999);
}

TEST(JsonParserTest, SparseArray) {
    std::istringstream json(R"({"array": [1, null, 3, null, 5]})");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("array").value().Get(0).value().Get<double>(), 1);
    EXPECT_EQ(result.Get("array").value().Get(1).value().Get<double>(),
              std::nullopt);
    EXPECT_EQ(result.Get("array").value().Get(2).value().Get<double>(), 3);
    EXPECT_EQ(result.Get("array").value().Get(3).value().Get<double>(),
              std::nullopt);
    EXPECT_EQ(result.Get("array").value().Get(4).value().Get<double>(), 5);
}

TEST(JsonParserTest, ComplexNestedStructure) {
    std::string json = R"({
        "company": {
            "name": "TechCorp",
            "employees": [
                {"id": 1, "name": "Alice", "skills": ["C++", "Python"]},
                {"id": 2, "name": "Bob", "skills": ["Java", "JavaScript"]},
                {"id": 3, "name": "Charlie", "skills": []}
            ],
            "address": {"city": "Toronto", "country": "Canada"}
        }
    })";
    auto result = parseJSON(std::istringstream(json));
    EXPECT_EQ(
        result.Get("company").value().Get("name").value().Get<std::string>(),
        "TechCorp");
    EXPECT_EQ(result.Get("company")
                  .value()
                  .Get("employees")
                  .value()
                  .Get(0)
                  .value()
                  .Get("skills")
                  .value()
                  .Get(0)
                  .value()
                  .Get<std::string>(),
              "C++");
    EXPECT_EQ(result.Get("company")
                  .value()
                  .Get("employees")
                  .value()
                  .Get(2)
                  .value()
                  .Get("skills")
                  .value()
                  .Get(0),
              std::nullopt);
    EXPECT_EQ(result.Get("company")
                  .value()
                  .Get("address")
                  .value()
                  .Get("city")
                  .value()
                  .Get<std::string>(),
              "Toronto");
}

TEST(JsonParserTest, EscapedCharacters) {
    std::istringstream json(R"({
        "text": "Line1\\nLine2\\tTabbed\\\"Quote\\\"",
        "unicode": "汉字"
    })");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("text").value().Get<std::string>(),
              R"(Line1\\nLine2\\tTabbed\\\"Quote\\\")");
    EXPECT_EQ(result.Get("unicode").value().Get<std::string>(), "汉字");
}

TEST(JsonParserTest, UnicodeCharacters) {
    std::istringstream json(R"({
        "emoji": "😊",
        "chinese": "你好",
        "arabic": "مرحبا",
        "hebrew": "שלום"
    })");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("emoji").value().Get<std::string>(), "😊");
    EXPECT_EQ(result.Get("chinese").value().Get<std::string>(), "你好");
    EXPECT_EQ(result.Get("arabic").value().Get<std::string>(), "مرحبا");
    EXPECT_EQ(result.Get("hebrew").value().Get<std::string>(), "שלום");
}

TEST(JsonParserTest, LargeNumbers) {
    std::istringstream json(R"({
        "largeInt": 9223372036854775807,
        "largeFloat": 1.7976931348623157e+308
    })");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("largeInt").value().Get<double>(),
              9223372036854775807);
    EXPECT_DOUBLE_EQ(result.Get("largeFloat").value().Get<double>().value(),
                     1.7976931348623157e+308);
}

TEST(JsonParserTest, JSONWithComments) {
    std::istringstream json(R"({
        // This is a comment
        "key": "value", /* Another comment */
        "number": 42
    })");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("key").value().Get<std::string>(), "value");
    EXPECT_EQ(result.Get("number").value().Get<double>(), 42);
}

/*
 * Test Insert/Append/Update
 */

TEST(JsonParserTest, AddSimpleObject) {
    auto json = parseJSON("{}");
    json.InsertOrUpdate("key", "value");
    EXPECT_EQ(json.Get("key").value().Get<std::string>(), "value");
}

TEST(JsonParserTest, AddSimpleArray) {
    auto json = parseJSON("[]");
    json.AppendOrUpdate(Json::end, "value");
    EXPECT_EQ(json.Get(0).value().Get<std::string>(), "value");
}

TEST(JsonParserTest, AddNestedObject) {
    auto json = parseJSON("{}");
    json.InsertOrUpdate("user", parseJSON("{}"));
    json.Get("user").value().InsertOrUpdate("name", "Alice");
    EXPECT_EQ(json.Get("user").value().Get("name").value().Get<std::string>(),
              "Alice");
}

TEST(JsonParserTest, AddNestedArray) {
    auto json = parseJSON("[]");
    json.AppendOrUpdate(Json::end, parseJSON("[]"));
    json.Get(0).value().AppendOrUpdate(Json::end, "Alice");
    EXPECT_EQ(json.Get(0).value().Get(0).value().Get<std::string>(), "Alice");
}

TEST(JsonParserTest, AddToObjArray) {
    auto json = parseJSON(R"({"array": []})");
    json.Get("array").value().AppendOrUpdate(
        Json::end, 1); // "" indicates appending to an array
    json.Get("array").value().AppendOrUpdate(Json::end, 2);
    EXPECT_EQ(json.Get("array").value().Get(0).value().Get<double>(), 1);
    EXPECT_EQ(json.Get("array").value().Get(1).value().Get<double>(), 2);
}

TEST(JsonParserTest, ModifyObject) {
    auto json = parseJSON(R"({"key": "initial"})");
    json.InsertOrUpdate("key", "new value");
    EXPECT_EQ(json.Get("key").value().Get<std::string>(), "new value");
}

TEST(JsonParserTest, ModifyArray) {
    auto json = parseJSON(R"(["key", "initial"])");
    json.AppendOrUpdate(0, "new value");
    EXPECT_EQ(json.Get(0).value().Get<std::string>(), "new value");
}

TEST(JsonParserTest, ModifyNestedObject) {
    auto json = parseJSON(R"({"user": {"name": "Alice"}})");
    json.Get("user").value().InsertOrUpdate("name", "Bob");
    EXPECT_EQ(json.Get("user").value().Get("name").value().Get<std::string>(),
              "Bob");
}

TEST(JsonParserTest, ModifyNestedArray) {
    auto json = parseJSON(R"(["user", ["name", "Alice"]])");
    json.Get(1).value().AppendOrUpdate(0, "Bob");
    EXPECT_EQ(json.Get(1).value().Get(0).value().Get<std::string>(), "Bob");
}

TEST(JsonParserTest, ModifyArrayValue) {
    auto json = parseJSON(R"({"array": [1, 2, 3]})");
    json.Get("array").value().AppendOrUpdate(1,
                                             42); // Modify the second element
    EXPECT_EQ(json.Get("array").value().Get(1).value().Get<double>(), 42);
}

TEST(JsonParserTest, ModifyObjectValue) {
    auto json = parseJSON(R"([{"array": [1, 2, 3]}])");
    json.Get(0).value().Get("array").value().AppendOrUpdate(
        1,
        42); // Modify the second element
    EXPECT_EQ(
        json.Get(0).value().Get("array").value().Get(1).value().Get<double>(),
        42);
}

TEST(JsonParserTest, AddAndModifyObject) {
    auto json = parseJSON("{}");
    json.InsertOrUpdate("key", "initial");
    EXPECT_EQ(json.Get("key").value().Get<std::string>(), "initial");
    json.InsertOrUpdate("key", "modified");
    EXPECT_EQ(json.Get("key").value().Get<std::string>(), "modified");
}

TEST(JsonParserTest, AddAndModifyArray) {
    auto json = parseJSON("[]");
    json.AppendOrUpdate(0, "initial");
    EXPECT_EQ(json.Get(0).value().Get<std::string>(), "initial");
    json.AppendOrUpdate(0, "modified");
    EXPECT_EQ(json.Get(0).value().Get<std::string>(), "modified");
}

TEST(JsonParserTest, AddAndModifyNestedObject) {
    auto json = parseJSON(R"({"user": {}})");
    json.Get("user").value().InsertOrUpdate("age", 25);
    EXPECT_EQ(json.Get("user").value().Get("age").value().Get<double>(), 25);
    json.Get("user").value().InsertOrUpdate("age", 26);
    EXPECT_EQ(json.Get("user").value().Get("age").value().Get<double>(), 26);
}

TEST(JsonParserTest, AddAndModifyNestedArray) {
    auto json = parseJSON(R"([[]])");
    json.Get(0).value().AppendOrUpdate(Json::end, 25);
    EXPECT_EQ(json.Get(0).value().Get(0).value().Get<double>(), 25);
    json.Get(0).value().AppendOrUpdate(0, 26);
    EXPECT_EQ(json.Get(0).value().Get(0).value().Get<double>(), 26);
}

TEST(JsonParserTest, AddNullObject) {
    auto json = parseJSON("{}");
    json.InsertOrUpdate("key", JNull{});
    EXPECT_EQ(json.Get("key").value().Get<bool>(), std::nullopt);
}

TEST(JsonParserTest, AddNullArray) {
    auto json = parseJSON("[]");
    json.AppendOrUpdate(0, JNull{});
    EXPECT_EQ(json.Get(0).value().Get<bool>(), std::nullopt);
}

TEST(JsonParserTest, AddLargeData) {
    auto json = parseJSON("{}");
    std::string largeData(10000, 'x');
    json.InsertOrUpdate("large", largeData);
    EXPECT_EQ(json.Get("large").value().Get<std::string>(), largeData);
}

TEST(JsonParserTest, AddDeeplyNestedKey) {
    auto json = parseJSON("{}");
    auto nested = parseJSON("{}");
    nested.InsertOrUpdate("key", "value");
    json.InsertOrUpdate("level1", nested);
    EXPECT_EQ(json.Get("level1").value().Get("key").value().Get<std::string>(),
              "value");
}

TEST(JsonParserTest, ModifyType) {
    auto json = parseJSON(R"({"key": "value"})");
    json.InsertOrUpdate("key", 42);
    EXPECT_EQ(json.Get("key").value().Get<double>(), 42);
}
