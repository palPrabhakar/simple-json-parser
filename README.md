# Simple JSON Parser

A modern C++20 JSON parser and manipulation library with a focus on type safety and ease of use.

## Features

- **Full JSON Support**: Parse and generate JSON objects, arrays, strings, numbers, booleans, and null values
- **Modern C++20**: Utilizes concepts, templates, and modern C++ features for type safety
- **Dynamic Manipulation**: Create, modify, and update JSON structures at runtime
- **Type-Safe Access**: Template-based getter methods with compile-time type checking
- **Memory Efficient**: Uses smart pointers and RAII principles
- **Comprehensive Testing**: Extensive test suite covering edge cases and complex scenarios

## Supported JSON Types

- **Objects**: Key-value pairs with string keys
- **Arrays**: Ordered collections of values
- **Strings**: UTF-8 encoded text with proper escaping
- **Numbers**: Double-precision floating point
- **Booleans**: true/false values  
- **Null**: null values

## Quick Start

### Building

```bash
mkdir build && cd build
cmake ..
make
```

### Running Tests

```bash
# In build directory
make test
# Or run directly
./test_parser
```

### Basic Usage

```cpp
#include "json.hpp"
#include "parser.hpp"
#include <sstream>

using namespace sjp;

// Parse JSON from string
std::istringstream json_stream(R"({"name": "Alice", "age": 30})");
Parser parser(json_stream);
Json json = parser.Parse();

// Access values with type safety
auto name = json.Get("name").value().Get<std::string>(); // "Alice"
auto age = json.Get("age").value().Get<double>();        // 30

// Create and modify JSON
Json obj = JsonBuilder<JsonType::jobject>();
obj.InsertOrUpdate("key", "value");
obj.InsertOrUpdate("number", 42);

// Arrays
Json arr = JsonBuilder<JsonType::jarray>();
arr.AppendOrUpdate(Json::end, "item1");
arr.AppendOrUpdate(Json::end, "item2");

// Output JSON
json.Dump(std::cout); // Prints formatted JSON
```

## API Reference

### Core Classes

- **`Json`**: Main JSON value type that can hold any JSON data
- **`Parser`**: Parses JSON from input streams
- **`JsonObject`**: Represents JSON objects (key-value maps)
- **`JsonArray`**: Represents JSON arrays
- **`JsonValue<T>`**: Template for primitive JSON values

### Key Methods

#### Parsing
```cpp
Parser parser(input_stream);
Json json = parser.Parse();
```

#### Type-Safe Access
```cpp
// Get nested values
auto value = json.Get("key").value().Get<std::string>();
auto array_item = json.Get("array").value().Get(0).value().Get<double>();

// Check if key exists
if (auto opt_val = json.Get("optional_key")) {
    // Key exists, use opt_val.value()
}
```

#### Manipulation
```cpp
// Objects
json.InsertOrUpdate("key", "value");
json.InsertOrUpdate("nested", nested_json);

// Arrays  
json.AppendOrUpdate(Json::end, "append_to_end");
json.AppendOrUpdate(0, "replace_first_item");

// Dynamic type conversion
Json empty;
empty.InsertOrUpdate("key", "value"); // Converts to object
empty.AppendOrUpdate(0, "item");      // Converts to array
```

#### Utility
```cpp
size_t size = json.Size();        // Get container size
json.Dump(std::cout);            // Output formatted JSON
```

## Dependencies

- **C++20 compliant compiler** (clang++ recommended)
- **CMake 3.10+**
- **Google Test** (for running tests)

## Project Structure

```
├── include/
│   ├── json.hpp      # Core JSON data structures
│   ├── parser.hpp    # JSON parser interface
│   └── tokenizer.hpp # Lexical tokenizer
├── src/
│   ├── main.cpp      # Example usage
│   ├── parser.cpp    # Parser implementation
│   └── tokenizer.cpp # Tokenizer implementation
├── test/
│   └── parser_test.cpp # Comprehensive test suite
└── CMakeLists.txt    # Build configuration
```

## Testing

The project includes extensive tests covering:

- **Valid JSON**: Objects, arrays, nested structures, unicode
- **Invalid JSON**: Syntax errors, malformed input, edge cases  
- **Manipulation**: Insert, update, append operations
- **Performance**: Large JSON files and deeply nested structures
- **Edge Cases**: Empty containers, special characters, large numbers

Run tests with:
```bash
# Build and run tests
cmake --build build --target test

# Run specific test binary
./build/test_parser
```

## Examples

### Complex JSON Manipulation

```cpp
// Parse complex JSON
std::string json_str = R"({
    "users": [
        {"id": 1, "name": "Alice", "active": true},
        {"id": 2, "name": "Bob", "active": false}
    ],
    "metadata": {"version": "1.0"}
})";

std::istringstream stream(json_str);
Parser parser(stream);
Json json = parser.Parse();

// Access nested data
auto first_user = json.Get("users").value().Get(0).value();
auto name = first_user.Get("name").value().Get<std::string>();

// Modify existing data
first_user.InsertOrUpdate("active", false);

// Add new user
Json new_user = JsonBuilder<JsonType::jobject>();
new_user.InsertOrUpdate("id", 3);
new_user.InsertOrUpdate("name", "Charlie");
new_user.InsertOrUpdate("active", true);

json.Get("users").value().AppendOrUpdate(Json::end, std::move(new_user));

// Output result
json.Dump(std::cout);
```

## License

This project is available under standard open source terms. See the source files for specific licensing information.
