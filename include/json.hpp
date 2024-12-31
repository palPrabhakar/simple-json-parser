#pragma once

// json value types
enum class value_type {
  json_string,
  json_int,
  json_double,
  json_obj,
  json_array,
  json_true,
  json_false,
  json_null
};

template <typename T> struct json_type {
  using value = T;
};

using json_int = json_type<int>;
using json_double = json_type<double>;

// base class for json
class json {
public:
  json(value_type type) : type(type) {}

  template <typename T> T::value get_value() {
    using U = T::value;
    return U{};
  }

  value_type get_type() { return type; }

private:
  value_type type;
};
