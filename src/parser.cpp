#include <cassert>
#include <memory>

#include "json.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

Json Parser::Parse() {
    Json json;
    Token token = tokenizer.PeekToken();
    switch (token.type) {
    case TokenType::quoted_str:
        json = ParseQuotedString();
        break;
    case TokenType::number:
        json = ParseNumber();
        break;
    case TokenType::jbool:
        json = ParseBool();
        break;
    case TokenType::jnull:
        json = ParseNull();
        break;
    case TokenType::left_braces:
        json = ParseObject();
        break;
    case TokenType::left_bracket:
        json = ParseArray();
        break;
    default: {
        THROW_ERROR("Invalid JSON String");
    }
    }
    token = tokenizer.GetToken();
    if (token.type != TokenType::end) {
        THROW_ERROR("Invalid JSON String");
    }
    return json;
}

Json Parser::ParseQuotedString() {
    Token token = tokenizer.GetToken();
    if (token.type != TokenType::quoted_str) {
        THROW_ERROR("Expected quoted string");
    }
    return Json{.type = JsonType::jstring,
                .value = std::make_shared<JsonString>(
                    std::get<std::string>(token.value))};
}

Json Parser::ParseNumber() {
    Token token = tokenizer.GetToken();
    if (token.type != TokenType::number) {
        THROW_ERROR("Expected a number");
    }
    return Json{
        .type = JsonType::jnumber,
        .value = std::make_shared<JsonNumber>(std::get<double>(token.value))};
}

Json Parser::ParseBool() {
    Token token = tokenizer.GetToken();
    if (token.type != TokenType::jbool) {
        THROW_ERROR("Expected true|false");
    }
    return Json{.type = JsonType::jbool,
                .value =
                    std::make_shared<JsonBool>(std::get<bool>(token.value))};
}

Json Parser::ParseNull() {
    Token token = tokenizer.GetToken();
    if (token.type != TokenType::jnull) {
        THROW_ERROR("Expected null");
    }
    return Json{.type = JsonType::jnull,
                .value = std::make_shared<JsonNull>(JNull{})};
}

std::pair<std::string, Json> Parser::ParsePair() {
    if (tokenizer.PeekToken().type != TokenType::quoted_str) {
        THROW_ERROR("Error parsing json object - invalid key");
    }

    Token token = tokenizer.GetToken();
    auto key = std::get<std::string>(token.value);

    if (tokenizer.GetToken().type != TokenType::colon) {
        THROW_ERROR("Error parsing json object - expected ':'");
    }

    switch (tokenizer.PeekToken().type) {
    case TokenType::quoted_str:
        return {key, ParseQuotedString()};
    case TokenType::number:
        return {key, ParseNumber()};
    case TokenType::jbool:
        return {key, ParseBool()};
    case TokenType::jnull:
        return {key, ParseNull()};
    case TokenType::left_braces:
        return {key, ParseObject()};
    case TokenType::left_bracket:
        return {key, ParseArray()};
    default: {
        THROW_ERROR("Error parsing json object");
    }
    }
}

Json Parser::ParseObject() {
    if (tokenizer.GetToken().type != TokenType::left_braces) {
        THROW_ERROR("Error parsing json object - expected '{'");
    }
    std::unordered_map<std::string, Json> pairs;
    // This is required to parse empty objects!
    while (tokenizer.PeekToken().type != TokenType::right_braces) {
        auto [key, value] = ParsePair();
        if (pairs.contains(key)) {
            THROW_ERROR("Error duplicat key in json object");
        }
        pairs.emplace(key, std::move(value));

        switch (tokenizer.PeekToken().type) {
        case TokenType::comma: {
            tokenizer.GetToken();
            if (tokenizer.PeekToken().type == TokenType::right_braces) {
                THROW_ERROR("Error parsing json object - unexpected '}'");
            }
        } break;
        case TokenType::right_braces:
            break;
        default: {
            THROW_ERROR("Error parsing JSON object");
        }
        }
    }
    if(tokenizer.GetToken().type != TokenType::right_braces) {
        THROW_ERROR("Error parssing JSON object - expected '}'");
    }
    return Json{.type = JsonType::jobject,
                .value = std::make_shared<JsonObject>(std::move(pairs))};
}

Json Parser::ParseArray() {
    if (tokenizer.GetToken().type != TokenType::left_bracket) {
        THROW_ERROR("Error parsing json arry - expected '['");
    }
    std::vector<Json> arr;
    // This is required to parse empty arrays!
    while (tokenizer.PeekToken().type != TokenType::right_bracket) {
        switch (tokenizer.PeekToken().type) {
        case TokenType::quoted_str:
            arr.emplace_back(ParseQuotedString());
            break;
        case TokenType::number:
            arr.emplace_back(ParseNumber());
            break;
        case TokenType::jbool: {
            arr.emplace_back(ParseBool());
        } break;
        case TokenType::jnull: {
            arr.emplace_back(ParseNull());
        } break;
        case TokenType::left_braces: {
            arr.emplace_back(ParseObject());
        } break;
        case TokenType::left_bracket: {
            arr.emplace_back(ParseArray());
        } break;
        default: {
            THROW_ERROR("Error parsing JSON array");
        }
        }
        switch (tokenizer.PeekToken().type) {
        case TokenType::comma: {
            tokenizer.GetToken();
            if (tokenizer.PeekToken().type == TokenType::right_bracket) {
                THROW_ERROR("Error parsing json object - unexpected ']'");
            }
        } break;
        case TokenType::right_bracket:
            break;
        default: {
            THROW_ERROR("Error parsing JSON array");
        }
        }
    }
    if(tokenizer.GetToken().type != TokenType::right_bracket) {
        THROW_ERROR("Error parssing JSON object - expected ']'");
    }
    return Json{.type = JsonType::jarray,
                .value = std::make_shared<JsonArray>(std::move(arr))};
}
