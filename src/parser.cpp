#include <cassert>
#include <iostream>
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
        std::cerr << "Invalid JSON String" << std::endl;
        abort();
    }
    }
    token = tokenizer.GetToken();
    if (token.type != TokenType::end) {
        std::cerr << "Invalid JSON String" << std::endl;
        abort();
    }
    return json;
}

Json Parser::ParseQuotedString() {
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::quoted_str && "Expected Quoted String\n");
    return Json{.type = JsonType::jstring,
                .value = std::make_shared<JsonString>(
                    std::get<std::string>(token.value))};
}

Json Parser::ParseNumber() {
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::number && "Expected number\n");
    return Json{
        .type = JsonType::jnumber,
        .value = std::make_shared<JsonNumber>(std::get<double>(token.value))};
}

Json Parser::ParseBool() {
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::jbool && "Expected true|false\n");
    return Json{.type = JsonType::jbool,
                .value =
                    std::make_shared<JsonBool>(std::get<bool>(token.value))};
}

Json Parser::ParseNull() {
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::jnull && "Expected null\n");
    return Json{.type = JsonType::jnull,
                .value = std::make_shared<JsonNull>(JNull{})};
}

std::pair<std::string, Json> Parser::ParsePair() {
    assert(tokenizer.PeekToken().type == TokenType::quoted_str &&
           "Error parsing JSON pair: invalid Key\n");

    Token token = tokenizer.GetToken();

    auto key = std::get<std::string>(token.value);

    assert(tokenizer.GetToken().type == TokenType::colon &&
           "Error parsing JSON pair: expected colon\n");

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
        std::cerr << "Unexpected error parsing JSON object" << std::endl;
        abort();
    }
    }
}

Json Parser::ParseObject() {
    assert(tokenizer.GetToken().type == TokenType::left_braces &&
           "Expected '{'\n");

    std::unordered_map<std::string, Json> pairs;
    // This is required to parse empty objects!
    while (tokenizer.PeekToken().type != TokenType::right_braces) {
        auto [key, value] = ParsePair();
        if (pairs.contains(key)) {
            std::cerr << "Error duplicate key: " << key << std::endl;
            abort();
        }
        pairs.emplace(key, std::move(value));

        switch (tokenizer.PeekToken().type) {
        case TokenType::comma: {
            auto _ = tokenizer.GetToken();
            assert(tokenizer.PeekToken().type != TokenType::right_braces &&
                   "Unexpected error parsing JSON object");
        } break;
        case TokenType::right_braces:
            break;
        default: {
            std::cerr << "Unexpected error parsing JSON object" << std::endl;
            abort();
        }
        }
    }
    auto _ = tokenizer.GetToken();

    return Json{.type = JsonType::jobject,
                .value = std::make_shared<JsonObject>(std::move(pairs))};
}

Json Parser::ParseArray() {
    assert(tokenizer.GetToken().type == TokenType::left_bracket &&
           "Expected '['\n");
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
            std::cerr << "Unexpected error parsing JSON Array" << std::endl;
            abort();
        }
        }
        switch (tokenizer.PeekToken().type) {
        case TokenType::comma: {
            auto _ = tokenizer.GetToken();
            assert(tokenizer.PeekToken().type != TokenType::right_bracket &&
                   "Unexpected error parsing JSON Array");
        } break;
        case TokenType::right_bracket:
            break;
        default: {
            std::cerr << "Unexpected error parsing JSON Array" << std::endl;
            abort();
        }
        }
    }
    auto _ = tokenizer.GetToken();

    return Json{.type = JsonType::jarray,
                .value = std::make_shared<JsonArray>(std::move(arr))};
}
