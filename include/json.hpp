#pragma once

#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

enum class JsonType { jstring, jnumber, jnull, jbool, jobject, jarray };

// base class for json
class Base {
  public:
    struct Json {
        JsonType type;
        std::shared_ptr<Base> value;

        void dump() const { value->Print(); }

        std::optional<Json> get(size_t);      // json-array
        std::optional<Json> get(std::string); // json-object
        template <typename RetType>
        std::optional<RetType> get() const; // json-value
    };

    virtual ~Base() = default;
    void Print() { PrintImpl(); }

    template <int> std::optional<Json> get();

  protected:
    Base() = default;

  private:
    virtual void PrintImpl() { __builtin_unreachable(); }
};

template <typename ValueType> class JsonValue : public Base {
  public:
    JsonValue(ValueType val) : value(val) {}

    ValueType value;

  private:
    void PrintImpl() override { std::cout << value; }
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

    std::unordered_map<std::string, Json> value;

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
};

class JsonArray : public Base {
  public:
    JsonArray(std::vector<Json> val) : value(std::move(val)) {}

    std::vector<Json> value;

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
};

using Json = Base::Json;

__attribute__((__always_inline__)) inline std::optional<Json>
Json::get(size_t idx) {
    assert(type == JsonType::jarray);
    auto *ptr = static_cast<JsonArray *>(value.get());
    return idx < ptr->value.size() ? std::optional(ptr->value[idx])
                                   : std::nullopt;
};

__attribute__((__always_inline__)) inline std::optional<Json>
Json::get(std::string key) {
    assert(type == JsonType::jobject);
    auto *ptr = static_cast<JsonObject *>(value.get());
    return ptr->value.contains(key) ? std::optional(ptr->value[key])
                                    : std::nullopt;
}

template <typename RetType> std::optional<RetType> Json::get() const {
    // Handle null case
    if constexpr (std::is_same_v<std::string, RetType>) {
        if (type == JsonType::jstring) {
            auto *ptr = static_cast<JsonString *>(value.get());
            return ptr->value;
        } else {
            return std::nullopt;
        }
    } else if (std::is_same_v<double, RetType>) {
        if (type == JsonType::jnumber) {
            auto *ptr = static_cast<JsonNumber *>(value.get());
            return ptr->value;
        } else {
            return std::nullopt;
        }
    } else if (std::is_same_v<bool, RetType>) {
        assert(type == JsonType::jbool || type == JsonType::jnull);
        if (type == JsonType::jbool) {
            auto *ptr = static_cast<JsonBool *>(value.get());
            return ptr->value;
        } else {
            return std::nullopt;
        }
    } else {
        std::cerr << "Error Json::get() - invalid type" << std::endl;
        abort();
    }
}
