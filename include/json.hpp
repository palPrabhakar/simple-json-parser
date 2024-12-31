#pragma once

// json value types
enum class value_type {
  json_string,
  json_number,
  json_object,
  json_array,
  json_bool,
  json_null
};

template <typename T> struct json_type {
  using value = T;
};

using json_number = json_type<double>;

// base class for json
class Json {
public:
  Json(value_type type) : type(type) {}

  template <typename T> T::value get_value() {
    using U = T::value;
    return U{};
  }

  value_type get_type() { return type; }

private:
  value_type type;
};
