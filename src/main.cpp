#include <iostream>

#include "parser.hpp"

int main(void) {
    std::cout << "Simple JSON Parser" << std::endl;

    {
        std::istringstream ss("\"lol\"");
        std::cout << "InputString: " << ss.str() << std::endl;
        Parser parser(std::move(ss));
        auto json = parser.Parse();
        json.dump();
    }

    {
        std::istringstream ss("1.24");
        std::cout << "InputString: " << ss.str() << std::endl;
        Parser parser(std::move(ss));
        auto json = parser.Parse();
        json.dump();
    }

    {
        std::istringstream ss("true");
        std::cout << "InputString: " << ss.str() << std::endl;
        Parser parser(std::move(ss));
        auto json = parser.Parse();
        json.dump();
    }

    {
        std::istringstream ss("null");
        std::cout << "InputString: " << ss.str() << std::endl;
        Parser parser(std::move(ss));
        auto json = parser.Parse();
        json.dump();
    }

    {
        std::istringstream ss("{\"key \\\"1\":1}");
        std::cout << "InputString: " << ss.str() << std::endl;
        Parser parser(std::move(ss));
        auto json = parser.Parse();
        json.dump();
    }

    {
        std::istringstream ss("[true, null, 1]");
        std::cout << "InputString: " << ss.str() << std::endl;
        Parser parser(std::move(ss));
        auto json = parser.Parse();
        json.dump();
    }

    return 0;
}
