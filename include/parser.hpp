#pragma once

#include "json.hpp"
#include "tokenizer.hpp"
#include <utility>

class Parser {
  public:
    Parser(std::istringstream json_stream)
        : tokenizer(std::move(json_stream)) {}

    Json Parse();

  private:
    std::pair<std::string, Json> ParsePair();
    Json ParseQuotedString();
    Json ParseNumber();
    Json ParseBool();
    Json ParseNull();
    Json ParseObject();
    Json ParseArray();

    Tokenizer tokenizer;
};
