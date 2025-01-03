#pragma once

#include "json.hpp"
#include "tokenizer.hpp"

class Parser {
  public:
    Parser(std::istringstream json_stream)
        : tokenizer(std::move(json_stream)) {}

    Json Parse();

  private:
    Json ParseQuotedString();
    Json ParseNumber();
    Json ParseBool();
    Json ParseNull();
    JsonPair ParsePair();
    Json ParseObject();
    Json ParseArray();

    Tokenizer tokenizer;
};
