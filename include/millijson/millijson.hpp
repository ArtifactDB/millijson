#ifndef MILLIJSON_MILLIJSON_HPP
#define MILLIJSON_MILLIJSON_HPP

#include <memory>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <cmath>
#include <unordered_map>
#include <cstdio>

/**
 * @file millijson.hpp
 * @brief Header-only library for JSON parsing.
 */

/**
 * @namespace millijson
 * @brief A lightweight header-only JSON parser.
 */

namespace millijson {

/**
 * All known JSON types.
 */
enum Type {
    NUMBER,
    STRING,
    BOOLEAN,
    NOTHING,
    ARRAY,
    OBJECT
};

/**
 * @brief Virtual base class for all JSON types.
 */
struct Base {
    /**
     * @return Type of the JSON value.
     */
    virtual Type type() const = 0;

    /**
     * @cond
     */
    virtual ~Base() {}
    /**
     * @endcond
     */

    /**
     * @return The number, if `this` points to a `Number` class.
     */
    double get_number() const;

    /**
     * @return The string, if `this` points to a `String` class.
     */ 
    const std::string& get_string() const;

    /**
     * @return The boolean, if `this` points to a `Boolean` class.
     */ 
    bool get_boolean() const;

    /**
     * @return An unordered map of key-value pairs, if `this` points to an `Object` class.
     */ 
    const std::unordered_map<std::string, std::shared_ptr<Base> >& get_object() const;

    /**
     * @return A vector of `Base` objects, if `this` points to an `Array` class.
     */ 
    const std::vector<std::shared_ptr<Base> >& get_array() const;
};

/**
 * @brief JSON number.
 */
struct Number : public Base {
    /**
     * @cond
     */
    Number(double v) : value(v) {}
    /**
     * @endcond
     */

    Type type() const { return NUMBER; }

    /**
     * Value of the number.
     */
    double value;
};

/**
 * @brief JSON string.
 */
struct String : public Base {
    /**
     * @cond
     */
    String(std::string s) : value(std::move(s)) {}
    /**
     * @endcond
     */

    Type type() const { return STRING; }

    /**
     * Value of the string.
     */
    std::string value;
};

/**
 * @brief JSON boolean.
 */
struct Boolean : public Base {
    /**
     * @cond
     */
    Boolean(bool v) : value(v) {}
    /**
     * @endcond
     */

    Type type() const { return BOOLEAN; }

    /**
     * Value of the boolean.
     */
    bool value;
};

/**
 * @brief JSON null.
 */
struct Nothing : public Base {
    Type type() const { return NOTHING; }
};

/**
 * @brief JSON array.
 */
struct Array : public Base {
    Type type() const { return ARRAY; }

    /**
     * Contents of the array.
     */
    std::vector<std::shared_ptr<Base> > values;
};

/**
 * @brief JSON object.
 */
struct Object : public Base {
    Type type() const { return OBJECT; }

    /**
     * Key-value pairs of the object.
     */
    std::unordered_map<std::string, std::shared_ptr<Base> > values;
};

/**
 * @cond
 */
inline double Base::get_number() const {
    return static_cast<const Number*>(this)->value;
}

inline const std::string& Base::get_string() const {
    return static_cast<const String*>(this)->value;
}

inline bool Base::get_boolean() const {
    return static_cast<const Boolean*>(this)->value;
}

inline const std::unordered_map<std::string, std::shared_ptr<Base> >& Base::get_object() const {
    return static_cast<const Object*>(this)->values;
}

inline const std::vector<std::shared_ptr<Base> >& Base::get_array() const {
    return static_cast<const Array*>(this)->values;
}

template<class Input>
void chomp(Input& input) {
    while (input.valid() && std::isspace(input.get())) {
        input.advance();
    }
    return;
}

template<class Input>
bool is_expected_string(Input& input, const std::string& expected) {
    for (auto x : expected) {
        if (!input.valid()) {
            return false;
        }
        if (input.get() != x) {
            return false;
        }
        input.advance();
    }
    return true;
}

template<class Input>
std::string extract_string(Input& input) {
    size_t start = input.position() + 1;
    input.advance(); // get past the opening quote.
    std::string output;

    while (1) {
        char next = input.get();
        switch (next) {
            case '"':
                input.advance(); // get past the closing quote.
                return output;
            case '\\':
                input.advance();
                if (!input.valid()) {
                    throw std::runtime_error("unterminated string at position " + std::to_string(start));
                } else {
                    char next2 = input.get();
                    switch (next2) {
                        case '"':
                            output += '"';          
                            break;
                        case 'n':
                            output += '\n';
                            break;
                        case 'r':
                            output += '\r';
                            break;
                        case '\\':
                            output += '\\';
                            break;
                        case '/':
                            output += '/';
                            break;
                        case 'b':
                            output += '\b';
                            break;
                        case 'f':
                            output += '\f';
                            break;
                        case 't':
                            output += '\t';
                            break;
                        case 'u':
                            {
                                unsigned short mb = 0;
                                for (size_t i = 0; i < 4; ++i) {
                                    input.advance();
                                    if (!input.valid()){
                                        throw std::runtime_error("unterminated string at position " + std::to_string(start));
                                    }
                                    mb *= 16;
                                    char val = input.get();
                                    switch (val) {
                                        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                                            mb += val - '0';
                                            break;
                                        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': 
                                            mb += (val - 'a') + 10;
                                            break;
                                        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': 
                                            mb += (val - 'A') + 10;
                                            break;
                                        default:
                                            throw std::runtime_error("invalid unicode escape detected at position " + std::to_string(input.position() + 1));
                                    }
                                }

                                // Converting manually from UTF-16 to UTF-8. We only allow
                                // 3 bytes at most because there's only 4 hex digits in JSON. 
                                if (mb <= 127) {
                                    output += static_cast<char>(mb);
                                } else if (mb <= 2047) {
                                    unsigned char left = (mb >> 6) | 0b11000000;
                                    output += *(reinterpret_cast<char*>(&left));
                                    unsigned char right = (mb & 0b00111111) | 0b10000000;
                                    output += *(reinterpret_cast<char*>(&right));
                                } else {
                                    unsigned char left = (mb >> 12) | 0b11100000;
                                    output += *(reinterpret_cast<char*>(&left));
                                    unsigned char middle = ((mb >> 6) & 0b00111111) | 0b10000000;
                                    output += *(reinterpret_cast<char*>(&middle));
                                    unsigned char right = (mb & 0b00111111) | 0b10000000;
                                    output += *(reinterpret_cast<char*>(&right));
                                }
                            }
                            break;
                        default:
                            throw std::runtime_error("unrecognized escape '\\" + std::string(1, next2) + "'");
                    }
                }
                break;
            default:
                output += next;
                break;
        }

        input.advance();
        if (!input.valid()) {
            throw std::runtime_error("unterminated string at position " + std::to_string(start));
        }
    }

    return output; // Technically unreachable, but whatever.
}

template<class Input>
double extract_number(Input& input) {
    size_t start = input.position() + 1;
    double value = 0;
    double fractional = 10;
    double exponent = 0; 
    bool negative_exponent = false;

    auto is_terminator = [](char v) -> bool {
        return v == ',' || v == ']' || v == '}' || std::isspace(v);
    };

    auto finalizer = [&]() -> double {
        if (exponent) {
            if (negative_exponent) {
                exponent *= -1;
            }
            return value * std::pow(10.0, exponent);
        } else {
            return value;
        }
    };

    bool in_fraction = false;
    bool in_exponent = false;

    // We assume we're starting from the absolute value, after removing any preceding negative sign. 
    char lead = input.get();
    if (lead == '0') {
        input.advance();
        if (!input.valid()) {
            return 0;
        }

        char val = input.get();
        if (val == '.') {
            in_fraction = true;
        } else if (val == 'e' || val == 'E') {
            in_exponent = true;
        } else if (is_terminator(val)) {
            return finalizer();
        } else {
            throw std::runtime_error("invalid number starting with 0 at position " + std::to_string(start));
        }

    } else if (std::isdigit(lead)) {
        value += lead - '0';
        input.advance();

        while (input.valid()) {
            char val = input.get();
            if (val == '.') {
                in_fraction = true;
                break;
            } else if (val == 'e' || val == 'E') {
                in_exponent = true;
                break;
            } else if (is_terminator(val)) {
                return finalizer();
            } else if (!std::isdigit(val)) {
                throw std::runtime_error("invalid number containing '" + std::string(1, val) + "' at position " + std::to_string(start));
            }
            value *= 10;
            value += val - '0';
            input.advance();
        }

    } else {
        // this should never happen, as extract_number is only called when the lead is a digit (or '-').
    }

    if (in_fraction) {
        input.advance();
        if (!input.valid()) {
            throw std::runtime_error("invalid number with trailing '.' at position " + std::to_string(start));
        }

        char val = input.get();
        if (!std::isdigit(val)) {
            throw std::runtime_error("'.' must be followed by at least one digit at position " + std::to_string(start));
        }
        value += (val - '0') / fractional;

        input.advance();
        while (input.valid()) {
            char val = input.get();
            if (val == 'e' || val == 'E') {
                in_exponent = true;
                break;
            } else if (is_terminator(val)) {
                return finalizer();
            } else if (!std::isdigit(val)) {
                throw std::runtime_error("invalid number containing '" + std::string(1, val) + "' at position " + std::to_string(start));
            }
            fractional *= 10;
            value += (val - '0') / fractional;
            input.advance();
        } 
    }

    if (in_exponent) {
        input.advance();
        if (!input.valid()) {
            throw std::runtime_error("invalid number with trailing 'e/E' at position " + std::to_string(start));
        }

        char val = input.get();
        if (!std::isdigit(val)) {
            if (val == '-') {
                negative_exponent = true;
            } else if (val != '+') {
                throw std::runtime_error("'e/E' must be followed by a sign in number at position " + std::to_string(start));
            }
            input.advance();

            if (!input.valid()) {
                throw std::runtime_error("invalid number with trailing exponent sign at position " + std::to_string(start));
            }
            val = input.get();
            if (!std::isdigit(val)) {
                throw std::runtime_error("exponent sign must be followed by at least one digit in number at position " + std::to_string(start));
            }
        }

        exponent += (val - '0');
        input.advance();
        while (input.valid()) {
            char val = input.get();
            if (is_terminator(val)) {
                return finalizer();
            } else if (!std::isdigit(val)) {
                throw std::runtime_error("invalid number containing '" + std::string(1, val) + "' at position " + std::to_string(start));
            }
            exponent *= 10;
            exponent += (val - '0');
            input.advance();
        } 
    }

    return finalizer();
}

template<class Input>
std::shared_ptr<Base> parse_thing(Input& input) {
    std::shared_ptr<Base> output;

    size_t start = input.position() + 1;
    const char current = input.get();

    if (current == 't') {
        if (!is_expected_string(input, "true")) {
            throw std::runtime_error("expected a 'true' string at position " + std::to_string(start));
        }
        output.reset(new Boolean(true));

    } else if (current == 'f') {
        if (!is_expected_string(input, "false")) {
            throw std::runtime_error("expected a 'false' string at position " + std::to_string(start));
        }
        output.reset(new Boolean(false));

    } else if (current == 'n') {
        if (!is_expected_string(input, "null")) {
            throw std::runtime_error("expected a 'null' string at position " + std::to_string(start));
        }
        output.reset(new Nothing);

    } else if (current == '"') {
        output.reset(new String(extract_string(input)));

    } else if (current == '[') {
        auto ptr = new Array;
        output.reset(ptr);

        input.advance();
        chomp(input);
        if (!input.valid()) {
            throw std::runtime_error("unterminated array starting at position " + std::to_string(start));
        }

        if (input.get() != ']') {
            while (1) {
                ptr->values.push_back(parse_thing(input));

                chomp(input);
                if (!input.valid()) {
                    throw std::runtime_error("unterminated array starting at position " + std::to_string(start));
                }

                char next = input.get();
                if (next == ']') {
                    break;
                } else if (next != ',') {
                    throw std::runtime_error("unknown character '" + std::string(1, next) + "' in array at position " + std::to_string(input.position() + 1));
                }

                input.advance(); 
                chomp(input);
                if (!input.valid()) {
                    throw std::runtime_error("unterminated array starting at position " + std::to_string(start));
                }
            }
        }

        input.advance(); // skip the closing bracket.

    } else if (current == '{') {
        auto ptr = new Object;
        output.reset(ptr);
        auto& map = ptr->values;

        input.advance();
        chomp(input);
        if (!input.valid()) {
            throw std::runtime_error("unterminated object starting at position " + std::to_string(start));
        }

        if (input.get() != '}') {
            while (1) {
                char next = input.get();
                if (next != '"') {
                    throw std::runtime_error("expected a string as the object key at position " + std::to_string(input.position() + 1));
                }
                auto key = extract_string(input);
                if (map.find(key) != map.end()) {
                    throw std::runtime_error("detected duplicate keys in the object at position " + std::to_string(input.position() + 1));
                }

                chomp(input);
                if (!input.valid()) {
                    throw std::runtime_error("unterminated object starting at position " + std::to_string(start));
                }
                if (input.get() != ':') {
                    throw std::runtime_error("expected ':' to separate keys and values at position " + std::to_string(input.position() + 1));
                }

                input.advance();
                chomp(input);
                if (!input.valid()) {
                    throw std::runtime_error("unterminated object starting at position " + std::to_string(start));
                }
                map[key] = parse_thing(input);

                chomp(input);
                if (!input.valid()) {
                    throw std::runtime_error("unterminated object starting at position " + std::to_string(start));
                }

                next = input.get();
                if (next == '}') {
                    break;
                } else if (next != ',') {
                    throw std::runtime_error("unknown character '" + std::string(1, next) + "' in array at position " + std::to_string(input.position() + 1));
                }

                input.advance(); 
                chomp(input);
                if (!input.valid()) {
                    throw std::runtime_error("unterminated object starting at position " + std::to_string(start));
                }
            }
        }

        input.advance(); // skip the closing brace.

    } else if (current == '-') {
        input.advance(); 
        if (!input.valid()) {
            throw std::runtime_error("incomplete number starting at position " + std::to_string(start));
        }
        output.reset(new Number(-extract_number(input)));

    } else if (std::isdigit(current)) {
        output.reset(new Number(extract_number(input)));

    } else {
        throw std::runtime_error(std::string("unknown type starting with '") + std::string(1, current) + "' at position " + std::to_string(start));
    }

    return output;
}
/**
 * @endcond
 */

/**
 * @tparam Input Any class that provides the following methods:
 *
 * - `char get() const `, which extracts a `char` from the input source without advancing the position on the byte stream.
 * - `bool valid() const`, to determine whether an input `char` can be `get()` from the input.
 * - `void advance()`, to advance the input stream.
 * - `size_t position() const`, for the current position relative to the start of the byte stream.
 *
 * @param input An instance of an `Input` class, referring to the bytes from a JSON-formatted file or string.
 * @return A pointer to a JSON value.
 */
template<class Input>
std::shared_ptr<Base> parse(Input& input) {
    chomp(input);
    auto output = parse_thing(input);
    chomp(input);
    if (input.valid()) {
        throw std::runtime_error("invalid json with trailing non-space characters at position " + std::to_string(input.position() + 1));
    }
    return output;
}

/**
 * @param[in] ptr Pointer to an array containing a JSON string.
 * @param len Length of the array.
 * @return A pointer to a JSON value.
 */
inline std::shared_ptr<Base> parse_string(const char* ptr, size_t len) {
    struct RawReader {
        RawReader(const char* p, size_t n) : ptr_(p), len_(n) {}
        size_t pos_ = 0;
        const char * ptr_;
        size_t len_;

        char get() const {
            return ptr_[pos_];
        }

        bool valid() const {
            return pos_ < len_;
        }

        void advance() {
            ++pos_;
        }

        size_t position() const {
            return pos_;
        }
    };
    RawReader input(ptr, len);
    return parse(input);
}

/**
 * @param[in] path Pointer to an array containing a path to a JSON file.
 * @param buffer_size Size of the buffer to use for reading the file.
 * @return A pointer to a JSON value.
 */
inline std::shared_ptr<Base> parse_file(const char* path, size_t buffer_size = 65536) {
    struct FileReader{
        FileReader(const char* p, size_t b) : handle(std::fopen(p, "rb")), buffer(b) {
            if (!handle) {
                throw std::runtime_error("failed to open file at '" + std::string(p) + "'");
            }
            fill();
        }

        ~FileReader() {
            std::fclose(handle);
        }

        FILE* handle;
        std::vector<char> buffer;
        size_t available = 0;
        size_t index = 0;
        size_t overall = 0;

        char get() const {
            return buffer[index];
        }

        bool valid() const {
            return index < available;
        }

        void advance() {
            ++index;
            if (index < available) {
                return;
            }

            index = 0;
            overall += available;
            fill();
        }

        void fill() {
            available = std::fread(buffer.data(), sizeof(char), buffer.size(), handle);
            if (available != buffer.size()) {
                if (!std::feof(handle)) {
                    throw std::runtime_error("failed to read raw binary file (fread error " + std::to_string(std::ferror(handle)) + ")");
                }
            }
        }

        size_t position() const {
            return overall + index;
        }
    };

    FileReader input(path, buffer_size);
    return parse(input);
}

}

#endif