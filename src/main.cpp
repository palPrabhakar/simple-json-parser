#include <iostream>

#include "parser.hpp"

int main(void) {
    std::cout << "Simple JSON Parser" << std::endl;
    std::istringstream ss("{\"key \\\"1\":1}");

    std::cout<<"InputString: "<<ss.str()<<std::endl;

    // std::istringstream ss("[1]");
    Parser parser(std::move(ss));
    parser.Parse();

    return 0;
}
