#include <iostream>

#include "parser.hpp"

#define TEST(x)                                                                \
    std::cout << "InputString: " << x.str() << std::endl;                      \
    Parser parser(std::move(x));                                               \
    auto json = parser.Parse();                                                \
    json.dump();                                                               \
    std::cout << std::endl;

int main(void) {
    std::cout << "Simple JSON Parser" << std::endl;

    // {
    //     std::istringstream ss("\"lol\"");
    //     TEST(ss);
    // }

    // {
    //     std::istringstream ss("1.24");
    //     TEST(ss);
    // }

    // {
    //     std::istringstream ss("true");
    //     TEST(ss);
    // }

    // {
    //     std::istringstream ss("null");
    //     TEST(ss);
    // }

    // {
    //     std::istringstream ss("{\"key \\\"1\":1}");
    //     TEST(ss);
    // }

    // {
    //     std::istringstream ss(
    //         "{\"key1\":1, \"key2\": \"lol\", \"key3\": null}");
    //     TEST(ss);
    // }

    {
        std::istringstream ss("[true, null, 1]");
        // TEST(ss);
        Parser parser(std::move(ss));
        auto json = parser.Parse();
        auto &j = json.get(0);
        std::cout<<*j.get<bool>();
        std::cout<<std::endl;
    }

    // {
    //     std::istringstream ss(
    //         "{\"key1\":1, \"key2\": \"lol\", \"key3\": null, \"key4\": "
    //         "{\"key1\":1, \"key2\": \"lol\", \"key3\": null}}");
    //     TEST(ss);
    // }

    // {
    //     std::istringstream ss("[true, null, 1, [true, null, 1]]");
    //     TEST(ss);
    // }

    return 0;
}
