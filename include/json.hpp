#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

enum class JsonType { jstring, jnumber, jnull, jbool, jobject, jarray };
struct JNull {};

// base class for json
class Base {
  public:
    struct Json {
        JsonType type;
        std::shared_ptr<Base> value;

        void Dump() const { value->Print(); }

        std::optional<Json> Get(size_t idx) const { // json-array
            return value->Get(idx);
        }
        std::optional<Json> Get(std::string key) const { // json-object
            return value->Get(key);
        }
        template <typename RetType> std::optional<RetType> Get() const {
            if constexpr (std::is_same_v<RetType, std::string>) {
                return value->GetString();
            } else if constexpr (std::is_same_v<RetType, bool>) {
                return value->GetBool();
            } else if constexpr (std::is_same_v<RetType, double>) {
                return value->GetNumber();
            } else {
                return std::nullopt;
            }
        }
    };

    virtual ~Base() = default;
    void Print() { PrintImpl(); }

  private:
    virtual void PrintImpl() = 0;
    virtual std::optional<Json> Get(size_t) { return std::nullopt; }
    virtual std::optional<Json> Get(std::string) { return std::nullopt; }
    virtual std::optional<std::string> GetString() { return std::nullopt; }
    virtual std::optional<double> GetNumber() { return std::nullopt; }
    virtual std::optional<bool> GetBool() { return std::nullopt; }
};

template <typename ValueType> class JsonValue : public Base {
  public:
    JsonValue(ValueType val) : value(val) {}

  private:
    ValueType value;
    void PrintImpl() override { std::cout << value; }

    std::optional<std::string> GetString() override {
        if constexpr (std::is_same_v<ValueType, std::string>) {
            return value;
        } else {
            return std::nullopt;
        }
    }
    std::optional<double> GetNumber() override {
        if constexpr (std::is_same_v<ValueType, double>) {
            return value;
        } else {
            return std::nullopt;
        }
    }
    std::optional<bool> GetBool() override {
        if constexpr (std::is_same_v<ValueType, bool>) {
            return value;
        } else {
            return std::nullopt;
        }
    }
};

template <> inline void JsonValue<JNull>::PrintImpl() { std::cout << "null"; }

template <> inline void JsonValue<bool>::PrintImpl() {
    std::cout << (value ? "true" : "false");
}

class JsonObject : public Base {
  public:
    JsonObject(std::unordered_map<std::string, Json> val)
        : value(std::move(val)) {}

  private:
    std::unordered_map<std::string, Json> value;
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

    std::optional<Json> Get(std::string key) override {
        return value.contains(key) ? std::optional(value[key]) : std::nullopt;
    }
};

class JsonArray : public Base {
  public:
    JsonArray(std::vector<Json> val) : value(std::move(val)) {}

  private:
    std::vector<Json> value;
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

    std::optional<Json> Get(size_t idx) override {
        return idx < value.size() ? std::optional(value[idx]) : std::nullopt;
    }
};

using Json = Base::Json;
using JsonNumber = JsonValue<double>;
using JsonString = JsonValue<std::string>;
using JsonBool = JsonValue<bool>;
using JsonNull = JsonValue<JNull>;
