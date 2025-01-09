#pragma once

#include <format>
#include <sstream>
#include <string>
#include <variant>

#define THROW_ERROR(msg)                                                       \
    throw std::runtime_error(                                                  \
        std::format("{} at {} in {}.", msg, __LINE__, __FILE__));

enum class TokenType {
    start,
    quoted_str,    // "*"
    number,        // 4.20
    jbool,         // true|false
    jnull,         // null
    left_braces,   // {
    right_braces,  // }
    left_bracket,  // [
    right_bracket, // ]
    comma,         // ,
    colon,         // :
    end
};

struct Token {
    TokenType type;
    std::variant<std::string, double, bool> value;
};

class Tokenizer {
  public:
    Tokenizer(std::istream &stream)
        : token_stream(stream), token(TokenType::start, {}) {
        Advance();
    }

    Token GetToken() {
        Token rtoken = token;
        Advance();
        return rtoken;
    }

    Token PeekToken() const { return token; }

  private:
    std::istream &token_stream;
    Token token;

    void Advance();
    void ReadValue();
    void ReadQuotedString();
    void SkipComments(bool);
};
