#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum class JsonType { jstring, jnumber, jnull, jbool, jobject, jarray };

// base class for json
class Base {
  public:
    virtual ~Base() = default;
    void Print() { PrintImpl(); }

  protected:
    Base() = default;

  private:
    virtual void PrintImpl() { __builtin_unreachable(); }
};

struct Json {
    JsonType type;
    std::unique_ptr<Base> value;

    void dump() { value->Print(); }
};

template <typename ValueType> class JsonValue : public Base {
  public:
    JsonValue(ValueType val) : value(val) {}

  private:
    void PrintImpl() override { std::cout << value; }

    ValueType value;
};

struct JNull {};

using JsonNull = JsonValue<JNull>;
template <> inline void JsonValue<JNull>::PrintImpl() { std::cout << "null"; }

using JsonBool = JsonValue<bool>;
template <> inline void JsonValue<bool>::PrintImpl() {
    std::cout << (value ? "true" : "false");
}

using JsonNumber = JsonValue<double>;
using JsonString = JsonValue<std::string>;

class JsonObject : public Base {
  public:
    JsonObject(std::unordered_map<std::string, Json> val)
        : value(std::move(val)) {}

  private:
    void PrintImpl() override {
        std::cout << "{";
        if (!value.empty()) {
            auto &[key, val] = *value.begin();
            std::cout << key << ": ";
            val.value->Print();
            for (auto i = ++value.begin(); i != value.end(); ++i) {
                std::cout << ", ";
                auto &[key, val] = *i;
                std::cout << key << ": ";
                val.value->Print();
            }
        }
        std::cout << "}";
    }

    std::unordered_map<std::string, Json> value;
};

class JsonArray : public Base {
  public:
    JsonArray(std::vector<Json> val) : value(std::move(val)) {}

  private:
    void PrintImpl() override {
        std::cout << "[";
        if (!value.empty()) {
            auto &val = value[0];
            val.value->Print();
            for (size_t i = 1; i < value.size(); ++i) {
                std::cout << ", ";
                auto &val = value[i];
                val.value->Print();
            }
        }
        std::cout << "]";
    }

    std::vector<Json> value;
};
