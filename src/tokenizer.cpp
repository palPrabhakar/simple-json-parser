#include "tokenizer.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>

void Tokenizer::Advance() {
    if (next_token.type == TokenType::end)
        return;

    char c;
    while (true) {
        c = token_stream.peek();
        if (c == std::istringstream::traits_type::eof()) {
            // cur_token.type = TokenType::end;
            next_token.type = TokenType::end;
            return;
        }
        switch (c) {
        case '{': {
            next_token.type = TokenType::left_braces;
            token_stream.get();
            return;
        }
        case '}': {
            next_token.type = TokenType::right_braces;
            token_stream.get();
            return;
        }
        case '[': {
            next_token.type = TokenType::left_bracket;
            token_stream.get();
            return;
        }
        case ']': {
            next_token.type = TokenType::right_bracket;
            token_stream.get();
            return;
        }
        case '"': {
            ReadQuotedString();
            return;
        }
        case ':': {
            next_token.type = TokenType::colon;
            token_stream.get();
            return;
        }
        default: {
            ReadValue();
            return;
        }
        }
    }
}

void Tokenizer::ReadQuotedString() {
    std::ostringstream value;
    // first quote
    token_stream.get();
    char c = token_stream.peek();
    while (c != '"' && c != std::istringstream::traits_type::eof()) {
        if (c == '\\') {
            value << static_cast<char>(token_stream.get());
            c = token_stream.peek();
            if (c != '"' || c != '\\') {
                std::cerr << "Invalid char after '\'" << std::endl;
                std::abort();
            }
        }
        value << static_cast<char>(token_stream.get());
        c = token_stream.peek();
    }
    if (c == std::istringstream::traits_type::eof()) {
        std::cerr << "Unexpected end of parsing." << std::endl;
        std::abort();
    }
    token_stream.get();
    next_token.type = TokenType::quoted_str;
    next_token.value = value.str();
}

void Tokenizer::ReadValue() {
    std::ostringstream value;
    char c = token_stream.peek();
    do {
        if (!std::isspace(c)) {
            value << static_cast<char>(c);
        }
        token_stream.get();
        c = token_stream.peek();
    } while (c != '}' && c != ',' && c != ']' &&
             c != std::istringstream::traits_type::eof());

    if (value.str() == "null") {
        next_token.type = TokenType::jnull;
        next_token.value = value.str();
    } else if (value.str() == "false" || value.str() == "true") {
        next_token.type = TokenType::jbool;
        next_token.value = value.str();
    } else {
        try {
            size_t len;
            double number = std::stod(value.str(), &len);
            if (len != value.str().size()) {
                std::cerr << "Not a number" << std::endl;
                std::abort();
            }
            next_token.type = TokenType::number;
            next_token.value = value.str();
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            std::abort();
        }
    }
}
