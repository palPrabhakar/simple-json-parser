#include <iostream>

#include "json.hpp"
#include "parser.hpp"

int main(void) {
    std::cout << "Simple JSON Parser" << std::endl;

    {
        std::istringstream ss(R"(["true", null, 1])");
        // TEST(ss);
        Parser parser(std::move(ss));
        auto json = parser.Parse();
        auto j = json.Get(0);
        j.value().Set("lol - const char *");
        std::cout<<j.value().Get<std::string>().value()<<std::endl;
        j.value().Set(42.0);
        std::cout<<j.value().Get<double>().value()<<std::endl;
        int i0 = 44;
        j.value().Set(i0);
        std::cout<<j.value().Get<double>().value()<<std::endl;
        j.value().Set(true);
        std::cout<<j.value().Get<bool>().value()<<std::endl;
        j.value().Set(JNull{});
        // std::cout<<j.value().Get<bool>().value()<<std::endl;
        const std::string lol = "lol-std::string";
        char lol0[] = "lol char *";
        json.Set(lol0);
        std::cout<<json.Get<std::string>().value()<<std::endl;
        json.Set(lol);
        std::cout<<json.Get<std::string>().value()<<std::endl;

    }

    return 0;
}
