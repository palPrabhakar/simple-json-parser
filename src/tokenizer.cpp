#include <cstdlib>
#include <sstream>
#include <stdexcept>

#include "tokenizer.hpp"

namespace sjp {
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
        case '\n':
        case ' ': {
            token_stream.get();
            break;
        }
        case '/': {
            token_stream.get();
            c = static_cast<char>(token_stream.peek());
            if (c == '/' || c == '*') {
                SkipComments(c == '*');
            } else {
                THROW_ERROR("Unexpected error parsing json string");
            }
            break;
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
            if (!(c != '"' || c != '\\' || c != '\n' || c == '\t')) {
                THROW_ERROR("Invalid char after escape sequence");
            }
        }
        value << static_cast<char>(token_stream.get());
        c = static_cast<char>(token_stream.peek());
    }
    if (c == std::istringstream::traits_type::eof()) {
        THROW_ERROR("Unexpected end of parsing");
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
                THROW_ERROR("Error parsing json number");
            }
            token.type = TokenType::number;
            token.value = number;
        } catch (const std::invalid_argument &e) {
            THROW_ERROR("Error parsing json number - invalid argument");
        } catch (const std::out_of_range &e) {
            THROW_ERROR("Error parsing json number - number out of range");
        }
    }
}

void Tokenizer::SkipComments(bool multi) {
    token_stream.get();
    // comment starts
    if (multi) {
        while (token_stream.peek() != std::istringstream::traits_type::eof()) {
            char c = static_cast<char>(token_stream.get());
            if (c == '*') {
                if (token_stream.peek() == '/') {
                    token_stream.get();
                    return;
                }
            }
        }
    } else {
        while (token_stream.peek() != std::istringstream::traits_type::eof()) {
            char c = static_cast<char>(token_stream.get());
            if (c == '\n') {
                return;
            }
        }
    }
    THROW_ERROR("Unexpected error parsing json string");
}
} // namespace sjp
