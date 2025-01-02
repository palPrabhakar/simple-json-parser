#pragma once

#include <sstream>
#include <string>
#include <variant>

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
    Tokenizer(std::istringstream stream)
        : token_stream(std::move(stream)), token(TokenType::start, {}) {
        Advance();
    }

    Token GetToken() {
        Token rtoken = token;
        Advance();
        return rtoken;
    }

    Token PeekToken() const { return token; }

  private:
    std::istringstream token_stream;
    Token token;

    void Advance();
    void ReadValue();
    void ReadQuotedString();
};
