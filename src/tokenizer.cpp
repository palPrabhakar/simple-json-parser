#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "tokenizer.hpp"

void Tokenizer::Advance() {
    if (token.type == TokenType::end)
        return;

    char c;
    while (true) {
        c = static_cast<char>(token_stream.peek());
        if (c == std::istringstream::traits_type::eof()) {
            token.type = TokenType::end;
            return;
        }
        switch (c) {
        case '{': {
            token.type = TokenType::left_braces;
            token_stream.get();
            return;
        }
        case '}': {
            token.type = TokenType::right_braces;
            token_stream.get();
            return;
        }
        case '[': {
            token.type = TokenType::left_bracket;
            token_stream.get();
            return;
        }
        case ']': {
            token.type = TokenType::right_bracket;
            token_stream.get();
            return;
        }
        case '"': {
            ReadQuotedString();
            return;
        }
        case ':': {
            token.type = TokenType::colon;
            token_stream.get();
            return;
        }
        case ',': {
            token.type = TokenType::comma;
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
    char c = static_cast<char>(token_stream.peek());
    while (c != '"' && c != std::istringstream::traits_type::eof()) {
        if (c == '\\') {
            value << static_cast<char>(token_stream.get());
            c = static_cast<char>(token_stream.peek());
            if (c != '"' && c != '\\') {
                std::cerr << "Invalid char after '\\' " << c << std::endl;
                std::abort();
            }
        }
        value << static_cast<char>(token_stream.get());
        c = static_cast<char>(token_stream.peek());
    }
    if (c == std::istringstream::traits_type::eof()) {
        std::cerr << "Unexpected end of parsing." << std::endl;
        std::abort();
    }
    token_stream.get();
    token.type = TokenType::quoted_str;
    token.value = value.str();
}

void Tokenizer::ReadValue() {
    std::ostringstream value;
    char c = static_cast<char>(token_stream.peek());
    do {
        if (!std::isspace(c)) {
            value << static_cast<char>(c);
        }
        token_stream.get();
        c = static_cast<char>(token_stream.peek());
    } while (c != '}' && c != ',' && c != ']' &&
             c != std::istringstream::traits_type::eof());

    if (value.str() == "null") {
        token.type = TokenType::jnull;
    } else if (value.str() == "true" || value.str() == "false") {
        token.type = TokenType::jbool;
        token.value = value.str() == "true";
    } else {
        try {
            size_t len;
            double number = std::stod(value.str(), &len);
            if (len != value.str().size()) {
                std::cerr << "Not a number" << std::endl;
                std::abort();
            }
            token.type = TokenType::number;
            token.value = number;
        } catch (std::invalid_argument const &e) {
            std::cerr << e.what() << std::endl;
            std::abort();
        } catch (std::out_of_range const &e) {
            std::cerr << e.what() << std::endl;
            std::abort();
        }
    }
}
