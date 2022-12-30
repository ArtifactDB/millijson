# Yet another JSON parser

![Unit tests](https://github.com/LTLA/millijson/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/LTLA/millijson/actions/workflows/doxygenate.yaml/badge.svg)
[![codecov](https://codecov.io/gh/LTLA/millijson/branch/master/graph/badge.svg?token=DLTMWKJAG3)](https://codecov.io/gh/LTLA/millijson)

## Overview

**millijson** is a header-only C++ library that provides a lightweight JSON parser.
It has no selling points other than being small and simple, if you could call those selling points.
I wrote it mostly for fun but also to provide a replacement for the [`nlohmann/json`](https://github.com/nlohmann/json) library -
while excellent, its single-header source code is pretty large (around 25 kLoCs, if I remember correctly) and I wanted something slimmer.
This is most noticeable when vendoring the file into various modules/packages, where C++ suddenly becomes the main language of the associated repository, and I didn't like that.
Yes, I'm _that_ petty.

## Quick start

Given a JSON-formatted string:

```cpp
#include <string>
#include "millijson/millijson.hpp"

std::string foo = "[ { \"foo\": \"bar\" }, 1e-2 ]";
auto ptr = millijson::parse_string(foo.c_str(), foo.size());

ptr->type(); // millijson::ARRAY
const auto& array = ptr->get_array(); // vector of pointers

array[0]->type(); // millijson::OBJECT
const auto& mapping = array[0]->get_object(); // unordered map

const auto& value = *(mapping.find("foo"));
value->type(); # millijson::STRING
const auto& string = value->get_string(); // std::string

array[1]->type(); // milljson::NUMBER
auto number = array[1]->get_number(); // double
```

The same works with a file:

```cpp
auto ptr = millijson::parse_file("some_json_file.json");
```

If you just want to validate a file, without using memory to load it:

```cpp
millijson::validate_file("some_json_file.json");
```

See the [reference documentation](https://ltla.github.io/millijson) for more details.

## Building projects

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```
include(FetchContent)

FetchContent_Declare(
  millijson 
  GIT_REPOSITORY https://github.com/LTLA/millijson
  GIT_TAG master # or any version of interest
)

FetchContent_MakeAvailable(millijson)
```

Then you can link to **millijson** to make the headers available during compilation:

```
# For executables:
target_link_libraries(myexe millijson)

# For libaries
target_link_libraries(mylib INTERFACE millijson)
```

Or you can just copy the [`millijson.hpp`](include/millijson/millijson.hpp) file into your source directory.

## Links

Parsing is based on the JSON standard described [here](https://www.json.org/json-en.html).

This interface was inspired by the implementation in [`nlohmann/json`](https://github.com/nlohmann/json).
