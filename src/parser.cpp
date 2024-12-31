#include <iostream>

#include "parser.hpp"

Json Parser::Parse() {
    Token token = tokenizer.PeekToken();
    while (token.type != TokenType::end) {
        switch (token.type) {
        case TokenType::quoted_str:
            return ParseQuotedString();
        case TokenType::number:
            return ParseNumber();
        case TokenType::jbool:
            return ParseBool();
        case TokenType::jnull:
            return ParseNull();
        case TokenType::left_braces:
            return ParseObject();
        case TokenType::left_bracket:
            return ParseArray();
        default: {
            std::cerr << "Invalid JSON String" << std::endl;
            abort();
        }
        }
    }
    __builtin_unreachable();
}

Json Parser::ParseQuotedString() {
    std::cout<<"ParseQuotedString"<<std::endl;
    return Json(value_type::json_string);
}

Json Parser::ParseNumber() {
    std::cout<<"ParseNumber"<<std::endl;
    return Json(value_type::json_number);
}

Json Parser::ParseBool() {
    std::cout<<"ParseBool"<<std::endl;
    return Json(value_type::json_bool);
}

Json Parser::ParseNull() {
    std::cout<<"ParseNull"<<std::endl;
    return Json(value_type::json_null);
}

Json Parser::ParseObject() {
    std::cout<<"ParseObject"<<std::endl;
    return Json(value_type::json_object);
}

Json Parser::ParseArray() {
    std::cout<<"ParseArray"<<std::endl;
    return Json(value_type::json_array);
}
