#include <iostream>

#include "json.hpp"
#include "tokenizer.hpp"

int main(void) {
    std::cout << "Simple JSON Parser" << std::endl;
    json _json(value_type::json_int);
    std::istringstream ss("{\"key\":1}");

    std::cout<<"InputString: "<<ss.str()<<std::endl;

    // std::istringstream ss("[1]");
    Tokenizer tokenizer(std::move(ss));
    while (tokenizer.PeekToken().type != TokenType::end) {
        Token token = tokenizer.GetToken();
        if (token.type == TokenType::number || token.type == TokenType::quoted_str)
            std::cout << token.value << std::endl;
    }
    // std::cout<<_json.get_value<json_int>()<<std::endl;
    // std::cout<<_json.get_value<json_double>()<<std::endl;
    return 0;
}
