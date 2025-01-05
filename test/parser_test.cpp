#include "parser.hpp"
#include <gtest/gtest.h>
#include <sstream>

Json parseJSON(std::istringstream json) {
    Parser parser(std::move(std::move(json)));
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

// TEST(JsonParserTest, SpecialCharacters) {
//     std::istringstream json(R"({"text": "Line1\nLine2\tIndented"})");
//     auto result = parseJSON(std::move(json));
//     EXPECT_EQ(result.Get("text").value().Get<std::string>(),
//     "Line1\nLine2\tIndented");
// }

// Invalid Json
// TEST(JsonParserTest, MissingBrace) {
//     std::istringstream json(R"({"key": "value")"); // Missing closing brace
//     EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
// }

// TEST(JsonParserTest, InvalidKeyFormat) {
//     std::istringstream json(R"({key: "value"})"); // Key not in quotes
//     EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
// }

// TEST(JsonParserTest, UnexpectedToken) {
//     std::istringstream json(R"({"key": "value", "extra"})"); // Invalid
//     syntax EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
// }

// TEST(JsonParserTest, InvalidNumber) {
//     std::istringstream json(R"({"number": 1e9999})"); // Out-of-range number
//     EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
// }

// TEST(JsonParserTest, UnterminatedString) {
//     std::istringstream json(
//         R"({"key": "value})"); // Missing closing quote for string
//     EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
// }

// edge cases
TEST(JsonParserTest, EmptyObject) {
    std::istringstream json(R"({})");
    EXPECT_NO_THROW(parseJSON(std::move(json)));
}

// TEST(JsonParserTest, OnlyWhitespace) {
//     std::istringstream json("   ");
//     EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
// }

// TEST(JsonParserTest, InvalidDataType) {
//     std::istringstream json(R"({"key": invalid})");
//     EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
// }

// TEST(JsonParserTest, DuplicateKeys) {
//     std::istringstream json(R"({"key": "value1", "key": "value2"})");
//     auto result = parseJSON(std::move(json));
//     EXPECT_EQ(result.Get("key").value(),
//               "value2"); // Last key-value pair should overwrite
// }

TEST(JsonParserTest, EmptyArray) {
    std::istringstream json(R"({"array": []})");
    EXPECT_NO_THROW(parseJSON(std::move(json)));
}

// complex cases
// TEST(JsonParserTest, DeeplyNestedObject) {
//     std::istringstream json(R"({
//         "level1": {
//             "level2": {
//                 "level3": {
//                     "level4": {
//                         "level5": {"key": "value"}
//                     }
//                 }
//             }
//         }
//     })");
//     auto result = parseJSON(std::move(json));
//     EXPECT_EQ(result.Get("level1")
//                   .value()
//                   .Get("level2")
//                   .value()
//                   .Get("level3")
//                   .value()
//                   .Get("level4")
//                   .value()
//                   .Get("level5")
//                   .value()
//                   .Get("key")
//                   .value()
//                   .Get<std::string>(),
//               "value");
// }

// TEST(JsonParserTest, LargeNestedArray) {
//     std::istringstream json(R"({"data": [[[[[[[1]]]]]]})");
//     auto result = parseJSON(std::move(json));
//     EXPECT_EQ(result.Get("data")
//                   .value()
//                   .Get(0)
//                   .value()
//                   .Get(0)
//                   .value()
//                   .Get(0)
//                   .value()
//                   .Get(0)
//                   .value()
//                   .Get(0)
//                   .value()
//                   .Get(0)
//                   .value()
//                   .Get(0)
//                   .value()
//                   .Get<double>(),
//               1);
// }

// TEST(JsonParserTest, MixedArrayAndObjects) {
//     std::istringstream json(R"({
//         "users": [
//             {"id": 1, "name": "Alice", "active": true},
//             {"id": 2, "name": "Bob", "active": false},
//             {"id": 3, "name": "Charlie", "active": true}
//         ]
//     })");
//     auto result = parseJSON(std::move(json));
//     // EXPECT_EQ(result.Get("users").size(), 3);
//     EXPECT_EQ(result.Get("users")
//                   .value()
//                   .Get(0)
//                   .value()
//                   .Get("name")
//                   .value()
//                   .Get<std::string>(),
//               "Alice");
//     EXPECT_EQ(result.Get("users")
//                   .value()
//                   .Get(1)
//                   .value()
//                   .Get("active")
//                   .value()
//                   .Get<bool>(),
//               false);
//     EXPECT_EQ(result.Get("users")
//                   .value()
//                   .Get(2)
//                   .value()
//                   .Get("id")
//                   .value()
//                   .Get<double>(),
//               3);
// }

// TEST(JsonParserTest, EscapedCharacters) {
//     std::istringstream json(R"({
//         "text": "Line1\\nLine2\\tTabbed\\\"Quote\\\"",
//         "unicode": "汉字"
//     })");
//     auto result = parseJSON(std::move(json));
//     EXPECT_EQ(result.Get("text"), "Line1\\nLine2\\tTabbed\\\"Quote\\\"");
//     EXPECT_EQ(result.Get("unicode"), "汉字");
// }

// TEST(JsonParserTest, LargeJSONFile) {
//     std::istringstream json(R"({"data": .Get()");
//     for (int i = 0; i < 100000; ++i) {
//         json += std::to_string(i) + ",";
//     }
//     json.back() = ')'; // Replace the last comma with a closing bracket
//     json += "}";
//     auto result = parseJSON(std::move(json));
//     // EXPECT_EQ(result.Get("data").size(), 100000);
//     EXPECT_EQ(result.Get("data").value().Get(0).value().Get<double>(), 0);
//     EXPECT_EQ(result.Get("data").value().Get(99999).value().Get<double>(), 99999);
// }

TEST(JsonParserTest, SparseArray) {
    std::istringstream json(R"({"array": [1, null, 3, null, 5]})");
    auto result = parseJSON(std::move(json));
    EXPECT_EQ(result.Get("array").value().Get(0).value().Get<double>(), 1);
    EXPECT_EQ(result.Get("array").value().Get(1).value().Get<double>(), std::nullopt);
    EXPECT_EQ(result.Get("array").value().Get(2).value().Get<double>(), 3);
    EXPECT_EQ(result.Get("array").value().Get(3).value().Get<double>(), std::nullopt);
    EXPECT_EQ(result.Get("array").value().Get(4).value().Get<double>(), 5);
}

// TEST(JsonParserTest, JSONWithComments) {
//     std::istringstream json(R"({
//         // This is a comment
//         "key": "value", /* Another comment */
//         "number": 42
//     })");
//     auto result = parseJSON(std::move(json));
//     EXPECT_EQ(result.Get("key").value().Get<std::string>(), "value");
//     EXPECT_EQ(result.Get("number").value().Get<double>(), 42);
// }

// TEST(JsonParserTest, ComplexNestedStructure) {
//     std::istringstream json(R"({
//         "company": {
//             "name": "TechCorp",
//             "employees": .Get(
//                 {"id": 1, "name": "Alice", "skills": .Get("C++", "Python")},
//                 {"id": 2, "name": "Bob", "skills": .Get("Java",
//                 "JavaScript")},
//                 {"id": 3, "name": "Charlie", "skills": .Get()}
//             ),
//             "address": {"city": "Toronto", "country": "Canada"}
//         }
//     })");
//     auto result = parseJSON(std::move(json));
//     EXPECT_EQ(result.Get("company").Get("name"), "TechCorp");
//     EXPECT_EQ(
//         result.Get("company").Get("employees").Get(0).Get("skills").Get(0),
//         "C++");
//     EXPECT_EQ(
//         result.Get("company").Get("employees").Get(2).Get("skills").size(),
//         0);
//     EXPECT_EQ(result.Get("company").Get("address").Get("city"), "Toronto");
// }

// TEST(JsonParserTest, UnicodeCharacters) {
//     std::istringstream json(R"({
//         "emoji": "😊",
//         "chinese": "你好",
//         "arabic": "مرحبا",
//         "hebrew": "שלום"
//     })");
//     auto result = parseJSON(std::move(json));
//     EXPECT_EQ(result.Get("emoji").value().Get<std::string>(), "😊");
//     EXPECT_EQ(result.Get("chinese").value().Get<std::string>(), "你好");
//     EXPECT_EQ(result.Get("arabic").value().Get<std::string>(), "مرحبا");
//     EXPECT_EQ(result.Get("hebrew").value().Get<std::string>(), "שלום");
// }

// TEST(JsonParserTest, TrailingComma) {
//     std::istringstream json(R"({"key1": "value1", "key2": "value2",})");
//     EXPECT_THROW(parseJSON(std::move(json)), std::runtime_error);
// }

// TEST(JsonParserTest, LargeNumbers) {
//     std::istringstream json(R"({
//         "largeInt": 9223372036854775807,
//         "largeFloat": 1.7976931348623157e+308
//     })");
//     auto result = parseJSON(std::move(json));
//     EXPECT_EQ(result.Get("largeInt"), 9223372036854775807);
//     EXPECT_DOUBLE_EQ(result.Get("largeFloat"), 1.7976931348623157e+308);
// }