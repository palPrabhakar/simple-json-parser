#pragma once

#include <cassert>
#include <concepts>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#define ESCAPE(x) "\"" << (x) << "\""

namespace sjp {
enum class JsonType { jstring, jnumber, jnull, jbool, jobject, jarray };

struct JNull {};
template <typename> class JsonValue;
using JsonNumber = JsonValue<double>;
using JsonString = JsonValue<std::string>;
using JsonBool = JsonValue<bool>;
using JsonNull = JsonValue<JNull>;

class Base;
template <JsonType> std::shared_ptr<Base> BaseBuilder();

template <typename T>
concept JVal =
    std::constructible_from<std::string, T> ||
    std::constructible_from<double, T> || std::constructible_from<JNull, T>;

// base class for json
class Base {
  public:
    struct Json {
        JsonType type;
        std::shared_ptr<Base> value;
        constexpr static size_t end = -1UL;

        void Dump(std::ostream &out = std::cout) const { value->Print(out); }

        size_t Size() const { return value->Size(); }

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

        template <typename T>
            requires JVal<T>
        void AppendOrUpdate(size_t idx, T val) {
            if (type != JsonType::jarray) {
                this->value = BaseBuilder<JsonType::jarray>();
                this->type = JsonType::jarray;
            }

            if constexpr (std::is_constructible_v<std::string, T>) {
                AppendOrUpdateString(idx, std::move(val));
            } else if constexpr (std::is_same_v<bool, T>) {
                AppendOrUpdateBool(idx, val);
            } else if constexpr (std::is_constructible_v<double, T>) {
                AppendOrUpdateNumber(idx, val);
            } else if constexpr (std::is_same_v<JNull, T>) {
                AppendOrUpdateNull(idx, val);
            } else {
                assert(false);
            }
        }

        void AppendOrUpdate(size_t idx, Json json) {
            AppendOrUpdateJson(idx, std::move(json));
        }

        template <typename T>
            requires JVal<T>
        void InsertOrUpdate(std::string key, T val) {
            if (type != JsonType::jobject) {
                this->value = BaseBuilder<JsonType::jobject>();
                this->type = JsonType::jobject;
            }

            if constexpr (std::is_constructible_v<std::string, T>) {
                InsertOrUpdateString(std::move(key), std::move(val));
            } else if constexpr (std::is_same_v<bool, T>) {
                InsertOrUpdateBool(std::move(key), val);
            } else if constexpr (std::is_constructible_v<double, T>) {
                InsertOrUpdateNumber(std::move(key), val);
            } else if constexpr (std::is_same_v<JNull, T>) {
                InsertOrUpdateNull(std::move(key), val);
            } else {
                assert(false);
            }
        }

        void InsertOrUpdate(std::string key, Json json) {
            InsertOrUpdateJson(std::move(key), std::move(json));
        }

      private:
        void AppendOrUpdateBool(size_t, bool);
        void AppendOrUpdateNull(size_t, JNull);
        void AppendOrUpdateNumber(size_t, double);
        void AppendOrUpdateString(size_t, std::string);
        void AppendOrUpdateJson(size_t, Json);
        void InsertOrUpdateBool(std::string, bool);
        void InsertOrUpdateNull(std::string, JNull);
        void InsertOrUpdateNumber(std::string, double);
        void InsertOrUpdateString(std::string, std::string);
        void InsertOrUpdateJson(std::string, Json);
    };

    virtual ~Base() = default;
    void Print(std::ostream &out) const { PrintImpl(out); }
    size_t Size() const { return SizeImpl(); }

  private:
    virtual void PrintImpl(std::ostream &) const = 0;
    virtual std::optional<Json> Get(size_t) { return std::nullopt; }
    virtual std::optional<Json> Get(std::string) { return std::nullopt; }
    virtual std::optional<std::string> GetString() { return std::nullopt; }
    virtual std::optional<double> GetNumber() { return std::nullopt; }
    virtual std::optional<bool> GetBool() { return std::nullopt; }
    virtual size_t SizeImpl() const { return 0; }
};

template <typename ValueType> class JsonValue : public Base {
  public:
    JsonValue(ValueType val) : value(val) {}
    ValueType value;

  private:
    void PrintImpl(std::ostream &out) const override {
        if constexpr (std::is_same_v<ValueType, std::string>) {
            out << ESCAPE(value);
        } else {
            out << value;
        }
    }

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

template <> inline void JsonValue<JNull>::PrintImpl(std::ostream &out) const {
    out << "null";
}

template <> inline void JsonValue<bool>::PrintImpl(std::ostream &out) const {
    out << (value ? "true" : "false");
}

class JsonObject : public Base {
  public:
    JsonObject(std::unordered_map<std::string, Json> val)
        : value(std::move(val)) {}

    void InsertOrUpdate(std::string key, Json val) {
        value[key] = std::move(val);
    }

  private:
    std::unordered_map<std::string, Json> value;

    void PrintImpl(std::ostream &out) const override {
        out << "{";
        if (!value.empty()) {
            auto &[key, val] = *value.begin();
            out << ESCAPE(key) << ": ";
            val.value->Print(out);
            for (auto i = ++value.begin(); i != value.end(); ++i) {
                out << ", ";
                auto &[key, val] = *i;
                out << ESCAPE(key) << ": ";
                val.value->Print(out);
            }
        }
        std::cout << "}";
    }

    size_t SizeImpl() const override { return value.size(); }

    std::optional<Json> Get(std::string key) override {
        return value.contains(key) ? std::optional(value[key]) : std::nullopt;
    }
};

class JsonArray : public Base {
  public:
    JsonArray(std::vector<Json> val) : value(std::move(val)) {}

    void AppendOrUpdate(size_t idx, Json json) {
        if (idx < value.size()) {
            value[idx] = std::move(json);
        } else {
            value.push_back(std::move(json));
        }
    }

  private:
    std::vector<Json> value;

    void PrintImpl(std::ostream &out) const override {
        out << "[";
        if (!value.empty()) {
            auto &val = value[0];
            val.value->Print(out);
            for (size_t i = 1; i < value.size(); ++i) {
                out << ", ";
                auto &val = value[i];
                val.value->Print(out);
            }
        }
        out << "]";
    }

    size_t SizeImpl() const override { return value.size(); }

    std::optional<Json> Get(size_t idx) override {
        return idx < value.size() ? std::optional(value[idx]) : std::nullopt;
    }
};

using Json = Base::Json;

template <JsonType type> std::shared_ptr<Base> BaseBuilder() {
    if constexpr (type == JsonType::jarray) {
        return std::make_shared<JsonArray>(std::vector<Json>{});
    } else if constexpr (type == JsonType::jobject) {
        return std::make_shared<JsonObject>(
            std::unordered_map<std::string, Json>{});
    } else if constexpr (type == JsonType::jstring) {
        return std::make_shared<JsonString>("");
    } else if constexpr (type == JsonType::jnumber) {
        return std::make_shared<JsonString>(0.0);
    } else if constexpr (type == JsonType::jbool) {
        return std::make_shared<JsonString>(false);
    } else {
        return std::make_shared<JsonNull>(JNull{});
    }
}

template <JsonType type> Json JsonBuilder() {
    return {.type = type, .value = BaseBuilder<type>()};
}

__attribute__((__always_inline__)) inline void
Json::AppendOrUpdateBool(size_t idx, bool val) {
    auto ptr = std::static_pointer_cast<JsonArray>(value);
    ptr->AppendOrUpdate(idx, {.type = JsonType::jbool,
                              .value = std::make_shared<JsonBool>(val)});
}

__attribute__((__always_inline__)) inline void
Json::AppendOrUpdateNull(size_t idx, JNull val) {
    auto ptr = std::static_pointer_cast<JsonArray>(value);
    ptr->AppendOrUpdate(idx, {.type = JsonType::jnull,
                              .value = std::make_shared<JsonNull>(val)});
}

__attribute__((__always_inline__)) inline void
Json::AppendOrUpdateNumber(size_t idx, double val) {
    auto ptr = std::static_pointer_cast<JsonArray>(value);
    ptr->AppendOrUpdate(idx, {.type = JsonType::jnumber,
                              .value = std::make_shared<JsonNumber>(val)});
}

__attribute__((__always_inline__)) inline void
Json::AppendOrUpdateString(size_t idx, std::string val) {
    auto ptr = std::static_pointer_cast<JsonArray>(value);
    ptr->AppendOrUpdate(
        idx, {.type = JsonType::jstring,
              .value = std::make_shared<JsonString>(std::move(val))});
}

__attribute__((__always_inline__)) inline void
Json::AppendOrUpdateJson(size_t idx, Json json) {
    auto ptr = std::static_pointer_cast<JsonArray>(value);
    ptr->AppendOrUpdate(idx, std::move(json));
}

__attribute__((__always_inline__)) inline void
Json::InsertOrUpdateBool(std::string key, bool val) {
    auto ptr = std::static_pointer_cast<JsonObject>(value);
    ptr->InsertOrUpdate(key, {.type = JsonType::jnull,
                              .value = std::make_shared<JsonBool>(val)});
}

__attribute__((__always_inline__)) inline void
Json::InsertOrUpdateNull(std::string key, JNull val) {
    auto ptr = std::static_pointer_cast<JsonObject>(value);
    ptr->InsertOrUpdate(key, {.type = JsonType::jnull,
                              .value = std::make_shared<JsonNull>(val)});
}

__attribute__((__always_inline__)) inline void
Json::InsertOrUpdateNumber(std::string key, double val) {
    auto ptr = std::static_pointer_cast<JsonObject>(value);
    ptr->InsertOrUpdate(key, {.type = JsonType::jnull,
                              .value = std::make_shared<JsonNumber>(val)});
}

__attribute__((__always_inline__)) inline void
Json::InsertOrUpdateString(std::string key, std::string val) {
    auto ptr = std::static_pointer_cast<JsonObject>(value);
    ptr->InsertOrUpdate(
        key, {.type = JsonType::jnull,
              .value = std::make_shared<JsonString>(std::move(val))});
}

__attribute__((__always_inline__)) inline void
Json::InsertOrUpdateJson(std::string key, Json json) {
    auto ptr = std::static_pointer_cast<JsonObject>(value);
    ptr->InsertOrUpdate(key, std::move(json));
}
} // namespace sjp
