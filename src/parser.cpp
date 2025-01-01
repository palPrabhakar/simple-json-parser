#include <cassert>
#include <iostream>

#include "json.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

Json Parser::Parse() {
    Json json(value_type::json_null);
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
    std::cout << "ParseQuotedString" << std::endl;
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::quoted_str && "Expected Quoted String\n");
    std::cout << "\tvalue: " << token.value << std::endl;
    return Json(value_type::json_string);
}

Json Parser::ParseNumber() {
    std::cout << "ParseNumber" << std::endl;
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::number && "Expected number\n");
    std::cout << "\tvalue: " << token.value << std::endl;
    return Json(value_type::json_number);
}

Json Parser::ParseBool() {
    std::cout << "ParseBool" << std::endl;
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::jbool && "Expected true|false\n");
    std::cout << "\tvalue: " << token.value << std::endl;
    return Json(value_type::json_bool);
}

Json Parser::ParseNull() {
    std::cout << "ParseNull" << std::endl;
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::jnull && "Expected null\n");
    std::cout << "\tvalue: " << token.value << std::endl;
    return Json(value_type::json_null);
}

Json Parser::ParseObject() {
    std::cout << "ParseObject" << std::endl;
    return Json(value_type::json_object);
}

Json Parser::ParseArray() {
    std::cout << "ParseArray" << std::endl;
    Token token = tokenizer.GetToken();
    assert(token.type == TokenType::left_bracket && "Expected '['\n");
    // This is required to parse empty arrays!
    while (tokenizer.PeekToken().type != TokenType::right_bracket) {
        switch (tokenizer.PeekToken().type) {
        case TokenType::quoted_str: {
            auto _ = ParseQuotedString();
        } break;
        case TokenType::number: {
            auto _ = ParseNumber();
        } break;
        case TokenType::jbool: {
            auto _ = ParseBool();
        } break;
        case TokenType::jnull: {
            auto _ = ParseNull();
        } break;
        case TokenType::left_braces: {
            auto _ = ParseObject();
        } break;
        case TokenType::left_bracket: {
            auto _ = ParseArray();
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

    return Json(value_type::json_array);
}
