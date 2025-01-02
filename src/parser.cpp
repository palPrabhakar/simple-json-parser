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
                .value = std::make_unique<JsonString>(
                    std::get<std::string>(token.value))};
}

Json Parser::ParseNumber() {
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::number && "Expected number\n");
    return Json{
        .type = JsonType::jnumber,
        .value = std::make_unique<JsonNumber>(std::get<double>(token.value))};
}

Json Parser::ParseBool() {
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::jbool && "Expected true|false\n");
    return Json{.type = JsonType::jbool,
                .value =
                    std::make_unique<JsonBool>(std::get<bool>(token.value))};
}

Json Parser::ParseNull() {
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::jnull && "Expected null\n");
    return Json{.type = JsonType::jnull,
                .value = std::make_unique<JsonNull>(JNull{})};
}

JsonPair Parser::ParsePair() {
    assert(tokenizer.PeekToken().type == TokenType::quoted_str &&
           "Error parsing JSON pair: invalid Key\n");

    JsonPair pair;

    Token token = tokenizer.GetToken();

    pair.key = std::get<std::string>(token.value);

    assert(tokenizer.GetToken().type == TokenType::colon &&
           "Error parsing JSON pair: expected colon\n");

    switch (tokenizer.PeekToken().type) {
    case TokenType::quoted_str:
        pair.value = ParseQuotedString();
        break;
    case TokenType::number:
        pair.value = ParseNumber();
        break;
    case TokenType::jbool:
        pair.value = ParseBool();
        break;
    case TokenType::jnull:
        pair.value = ParseNull();
        break;
    case TokenType::left_braces:
        pair.value = ParseObject();
        break;
    case TokenType::left_bracket:
        pair.value = ParseArray();
        break;
    default: {
        std::cerr << "Unexpected error parsing JSON object" << std::endl;
        abort();
    }
    }
    return pair;
}

Json Parser::ParseObject() {
    assert(tokenizer.GetToken().type == TokenType::left_braces &&
           "Expected '{'\n");

    std::vector<JsonPair> pairs;
    // This is required to parse empty objects!
    while (tokenizer.PeekToken().type != TokenType::right_braces) {
        pairs.emplace_back(ParsePair());
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
                .value = std::make_unique<JsonObject>(std::move(pairs))};
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
                .value = std::make_unique<JsonArray>(std::move(arr))};
}
