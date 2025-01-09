#pragma once

#include "json.hpp"
#include "tokenizer.hpp"
#include <utility>

namespace sjp {
class Parser {
  public:
    Parser(std::istream &json_stream) : tokenizer(json_stream) {}

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
} // namespace sjp
