#include <iostream>

#include "json.hpp"
#include "parser.hpp"

using namespace sjp;

int main(void) {
    std::cout << "Simple JSON Parser" << std::endl;

    {
        auto json =
            Json{.type = JsonType::jarray,
                 .value = std::make_shared<JsonArray>(std::vector<Json>{})};
        json.AppendOrUpdate(Json::end, 42);
        json.AppendOrUpdate(Json::end, true);
        json.AppendOrUpdate(Json::end, "lol");

        json.Dump();
        std::cout << std::endl;

        Json jarray = JsonBuilder<JsonType::jarray>();
        jarray.AppendOrUpdate(Json::end, "A");
        jarray.AppendOrUpdate(Json::end, "B");
        jarray.AppendOrUpdate(Json::end, "C");

        json.AppendOrUpdate(2, jarray);
        json.Dump();
        std::cout << std::endl;
    }

    // {
    //     std::istringstream ss(R"([1, [2, [3]]])");
    //     Parser parser(ss);
    //     auto json = parser.Parse();
    //     json.AppendOrUpdate(0, "lol");
    //     auto j1 = json.Get(1);
    //     j1.value().AppendOrUpdate(0, "lol");
    //     auto j2 = j1.value().Get(1);
    //     j2.value().AppendOrUpdate(0, "lol");
    //     json.Dump();
    //     std::cout << std::endl;
    // }

    {
        auto json = Json{.type = JsonType::jobject,
                         .value = std::make_shared<JsonObject>(
                             std::unordered_map<std::string, Json>{})};
        json.InsertOrUpdate("1", 42);
        json.InsertOrUpdate("2", true);
        json.InsertOrUpdate("3", "lol");
        json.Dump();
        std::cout << std::endl;

        auto jobj = JsonBuilder<JsonType::jobject>();
        jobj.InsertOrUpdate("1", "one");
        jobj.InsertOrUpdate("2", "two");
        jobj.InsertOrUpdate("3", "three");

        json.InsertOrUpdate("3", std::move(jobj));
        json.Dump();
        std::cout << std::endl;
    }

    // {
    //     std::istringstream ss(
    //         R"({"1": "1", "2": {"2": "2", "3": {"3": "3"}}})");
    //     Parser parser(ss);
    //     auto json = parser.Parse();
    //     json.InsertOrUpdate("1", "lol");
    //     auto j1 = json.Get("2");
    //     j1.value().InsertOrUpdate("2", "lol");
    //     auto j2 = j1.value().Get("3");
    //     j2.value().InsertOrUpdate("3", "lol");
    //     json.Dump();
    //     std::cout << std::endl;
    // }

    return 0;
}
