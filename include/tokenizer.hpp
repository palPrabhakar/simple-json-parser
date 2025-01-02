#pragma once

#include <sstream>
#include <string>

enum class TokenType {
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
    std::string value;
};

class Tokenizer {
  public:
    Tokenizer(std::istringstream stream) : token_stream(std::move(stream)) {
        Advance();
    }

    Token GetToken() {
        Token token = next_token;
        Advance();
        return token;
    }

    Token PeekToken() const { return next_token; }

  private:
    std::istringstream token_stream;
    // Token cur_token;
    Token next_token;

    void Advance();
    void ReadValue();
    void ReadQuotedString();

};
