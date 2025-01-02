#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum class JsonType { jstring, jnumber, jnull, jbool, jobject, jarray };

class Base;

struct Json {
    JsonType type;
    std::unique_ptr<Base> value;
};

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

template <typename ValueType> class JsonValue : public Base {
  public:
    JsonValue(ValueType val) : value(val) {}

  private:
    void PrintImpl() override { std::cout << "JsonValue" << std::endl; }

    ValueType value;
};

struct JNull {};

using JsonNull = JsonValue<JNull>;
using JsonBool = JsonValue<bool>;
using JsonNumber = JsonValue<double>;
using JsonString = JsonValue<std::string>;

struct JsonPair {
    std::string key;
    Json value;
};

class JsonObject : public Base {
  public:
    JsonObject(std::vector<JsonPair> val) : value(std::move(val)) {}

  private:
    void PrintImpl() override { std::cout << "JsonObject" << std::endl; }

    std::vector<JsonPair> value;
};

class JsonArray : public Base {
  public:
    JsonArray(std::vector<Json> val) : value(std::move(val)) {}

  private:
    void PrintImpl() override { std::cout << "JsonArray" << std::endl; }

    std::vector<Json> value;
};
