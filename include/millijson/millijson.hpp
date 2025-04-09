#ifndef MILLIJSON_MILLIJSON_HPP
#define MILLIJSON_MILLIJSON_HPP

#include <memory>
#include <vector>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
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
class Base {
public:
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
class Number final : public Base {
public:
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
class String final : public Base {
public:
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
class Boolean final : public Base {
public:
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
class Nothing final : public Base {
public:
    Type type() const { return NOTHING; }
};

/**
 * @brief JSON array.
 */
class Array final : public Base {
public:
    Type type() const { return ARRAY; }

    /**
     * Contents of the array.
     */
    std::vector<std::shared_ptr<Base> > values;

    /**
     * @param value Value to append to the array.
     */
    void add(std::shared_ptr<Base> value) {
        values.push_back(std::move(value));
        return;
    }
};

/**
 * @brief JSON object.
 */
class Object final : public Base {
public:
    Type type() const { return OBJECT; }

    /**
     * Key-value pairs of the object.
     */
    std::unordered_map<std::string, std::shared_ptr<Base> > values;

    /**
     * @param key String containing the key.
     * @return Whether `key` already exists in the object.
     */
    bool has(const std::string& key) const {
        return values.find(key) != values.end();
    }

    /**
     * @param key String containing the key.
     * @param value Value to add to the array.
     */
    void add(std::string key, std::shared_ptr<Base> value) {
        values[std::move(key)] = std::move(value);
        return;
    }
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

// Return value of the various chomp functions indicates whether there are any
// characters left in 'input', allowing us to avoid an extra call to valid(). 
template<class Input_>
bool raw_chomp(Input_& input, bool ok) {
    while (ok) {
        switch(input.get()) {
            // Allowable whitespaces as of https://www.rfc-editor.org/rfc/rfc7159#section-2.
            case ' ': case '\n': case '\r': case '\t':
                break;
            default:
                return true;
        }
        ok = input.advance();
    }
    return false;
}

template<class Input_>
bool check_and_chomp(Input_& input) {
    bool ok = input.valid();
    return raw_chomp(input, ok);
}

template<class Input_>
bool advance_and_chomp(Input_& input) {
    bool ok = input.advance();
    return raw_chomp(input, ok);
}

inline bool is_digit(char val) {
    return val >= '0' && val <= '9';
}

template<class Input_>
bool is_expected_string(Input_& input, const char* ptr, size_t len) {
    // We use a hard-coded 'len' instead of scanning for '\0' to enable loop unrolling.
    for (size_t i = 1; i < len; ++i) {
        // The current character was already used to determine what string to
        // expect, so we can skip past it in order to match the rest of the
        // string. This is also why we start from i = 1 instead of i = 0.
        if (!input.advance()) {
            return false;
        }
        if (input.get() != ptr[i]) {
            return false;
        }
    }
    input.advance(); // move off the last character.
    return true;
}

template<class Input_>
std::string extract_string(Input_& input) {
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
                if (!input.advance()) {
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
                                    if (!input.advance()){
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

                                // Manually convert Unicode code points to UTF-8. We only allow
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

            case (char) 0: case (char) 1: case (char) 2: case (char) 3: case (char) 4: case (char) 5: case (char) 6: case (char) 7: case (char) 8: case (char) 9:
            case (char)10: case (char)11: case (char)12: case (char)13: case (char)14: case (char)15: case (char)16: case (char)17: case (char)18: case (char)19:
            case (char)20: case (char)21: case (char)22: case (char)23: case (char)24: case (char)25: case (char)26: case (char)27: case (char)28: case (char)29:
            case (char)30: case (char)31:
            case (char)127:
                throw std::runtime_error("string contains ASCII control character at position " + std::to_string(input.position() + 1));

            default:
                output += next;
                break;
        }

        if (!input.advance()) {
            throw std::runtime_error("unterminated string at position " + std::to_string(start));
        }
    }

    return output; // Technically unreachable, but whatever.
}

template<class Input_>
double extract_number(Input_& input) {
    size_t start = input.position() + 1;
    double value = 0;
    bool in_fraction = false;
    bool in_exponent = false;

    // We assume we're starting from the absolute value, after removing any preceding negative sign.
    char lead = input.get();
    if (lead == '0') {
        if (!input.advance()) {
            return 0;
        }

        switch (input.get()) {
            case '.':
                in_fraction = true;
                break;
            case 'e': case 'E':
                in_exponent = true;
                break;
            case ',': case ']': case '}': case ' ': case '\r': case '\n': case '\t':
                return value;
            default:
                throw std::runtime_error("invalid number starting with 0 at position " + std::to_string(start));
        }

    } else { // 'lead' must be a digit, as extract_number is only called when the current character is a digit.
        value += lead - '0';

        bool finished = [&]{ // wrapping it in an IIFE to easily break out of the loop inside the switch.
            while (input.advance()) {
                char val = input.get();
                switch (input.get()) {
                    case '.':
                        in_fraction = true;
                        return false;
                    case 'e': case 'E':
                        in_exponent = true;
                        return false;
                    case ',': case ']': case '}': case ' ': case '\r': case '\n': case '\t':
                        return true;
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        value *= 10;
                        value += val - '0';
                        break;
                    default:
                        throw std::runtime_error("invalid number containing '" + std::string(1, val) + "' at position " + std::to_string(start));
                }
            }
            return true; // this is reached only when we run out of digits.
        }();

        if (finished) {
            return value;
        }
    }

    if (in_fraction) {
        if (!input.advance()) {
            throw std::runtime_error("invalid number with trailing '.' at position " + std::to_string(start));
        }

        char val = input.get();
        if (!is_digit(val)) {
            throw std::runtime_error("'.' must be followed by at least one digit at position " + std::to_string(start));
        }

        double fractional = 10;
        value += (val - '0') / fractional;

        bool finished = [&]{ // wrapping it in an IIFE to easily break out of the loop inside the switch.
            while (input.advance()) {
                char val = input.get();
                switch (input.get()) {
                    case 'e': case 'E':
                        in_exponent = true;
                        return false;
                    case ',': case ']': case '}': case ' ': case '\r': case '\n': case '\t':
                        return true;
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        fractional *= 10;
                        value += (val - '0') / fractional;
                        break;
                    default:
                        throw std::runtime_error("invalid number containing '" + std::string(1, val) + "' at position " + std::to_string(start));
                }
            } 
            return true; // should only be reached if we ran out of digits.
        }();

        if (finished) {
            return value;
        }
    }

    if (in_exponent) {
        double exponent = 0; 
        bool negative_exponent = false;

        if (!input.advance()) {
            throw std::runtime_error("invalid number with trailing 'e/E' at position " + std::to_string(start));
        }

        char val = input.get();
        if (!is_digit(val)) {
            if (val == '-') {
                negative_exponent = true;
            } else if (val != '+') {
                throw std::runtime_error("'e/E' should be followed by a sign or digit in number at position " + std::to_string(start));
            }

            if (!input.advance()) {
                throw std::runtime_error("invalid number with trailing exponent sign at position " + std::to_string(start));
            }
            val = input.get();
            if (!is_digit(val)) {
                throw std::runtime_error("exponent sign must be followed by at least one digit in number at position " + std::to_string(start));
            }
        }

        exponent += (val - '0');

        [&]{ // wrapping it in an IIFE to easily break out of the loop inside the switch.
            while (input.advance()) {
                char val = input.get();
                switch (val) {
                    case ',': case ']': case '}': case ' ': case '\r': case '\n': case '\t':
                        return;
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        exponent *= 10;
                        exponent += (val - '0');
                        break;
                    default:
                        throw std::runtime_error("invalid number containing '" + std::string(1, val) + "' at position " + std::to_string(start));
                }
            }
        }();

        if (exponent) {
            if (negative_exponent) {
                exponent *= -1;
            }
            value *= std::pow(10.0, exponent);
        }
    }

    return value;
}

struct DefaultProvisioner {
    typedef ::millijson::Base base;

    static Boolean* new_boolean(bool x) {
        return new Boolean(x); 
    }

    static Number* new_number(double x) {
        return new Number(x);
    }

    static String* new_string(std::string x) {
        return new String(std::move(x));
    }

    static Nothing* new_nothing() {
        return new Nothing;
    }

    static Array* new_array() {
        return new Array;
    }

    static Object* new_object() {
        return new Object;
    }
};

struct FakeProvisioner {
    class FakeBase {
    public:
        virtual Type type() const = 0;
        virtual ~FakeBase() {}
    };
    typedef FakeBase base;

    class FakeBoolean final : public FakeBase {
    public:
        Type type() const { return BOOLEAN; }
    };
    static FakeBoolean* new_boolean(bool) {
        return new FakeBoolean; 
    }

    class FakeNumber final : public FakeBase {
    public:    
        Type type() const { return NUMBER; }
    };
    static FakeNumber* new_number(double) {
        return new FakeNumber;
    }

    class FakeString final : public FakeBase {
    public:
        Type type() const { return STRING; }
    };
    static FakeString* new_string(std::string) {
        return new FakeString;
    }

    class FakeNothing final : public FakeBase {
    public:
        Type type() const { return NOTHING; }
    };
    static FakeNothing* new_nothing() {
        return new FakeNothing;
    }

    class FakeArray final : public FakeBase {
    public:
        Type type() const { return ARRAY; }
        void add(std::shared_ptr<FakeBase>) {}
    };
    static FakeArray* new_array() {
        return new FakeArray;
    }

    class FakeObject final : public FakeBase {
    public:
        Type type() const { return OBJECT; }
        std::unordered_set<std::string> keys;
        bool has(const std::string& key) const {
            return keys.find(key) != keys.end();
        }
        void add(std::string key, std::shared_ptr<FakeBase>) {
            keys.insert(std::move(key));
        }
    };
    static FakeObject* new_object() {
        return new FakeObject;
    }
};

template<class Provisioner_, class Input_>
std::shared_ptr<typename Provisioner_::base> parse_thing(Input_& input) {
    std::shared_ptr<typename Provisioner_::base> output;

    size_t start = input.position() + 1;
    const char current = input.get();

    switch(current) {
        case 't':
            if (!is_expected_string(input, "true", 4)) {
                throw std::runtime_error("expected a 'true' string at position " + std::to_string(start));
            }
            output.reset(Provisioner_::new_boolean(true));
            break;

        case 'f':
            if (!is_expected_string(input, "false", 5)) {
                throw std::runtime_error("expected a 'false' string at position " + std::to_string(start));
            }
            output.reset(Provisioner_::new_boolean(false));
            break;

        case 'n':
            if (!is_expected_string(input, "null", 4)) {
                throw std::runtime_error("expected a 'null' string at position " + std::to_string(start));
            }
            output.reset(Provisioner_::new_nothing());
            break;

        case '"': 
            output.reset(Provisioner_::new_string(extract_string(input)));
            break;

        case '[':
            {
                auto ptr = Provisioner_::new_array();
                output.reset(ptr);

                if (!advance_and_chomp(input)) {
                    throw std::runtime_error("unterminated array starting at position " + std::to_string(start));
                }

                if (input.get() != ']') {
                    while (1) {
                        ptr->add(parse_thing<Provisioner_>(input));

                        if (!check_and_chomp(input)) {
                            throw std::runtime_error("unterminated array starting at position " + std::to_string(start));
                        }

                        char next = input.get();
                        if (next == ']') {
                            break;
                        } else if (next != ',') {
                            throw std::runtime_error("unknown character '" + std::string(1, next) + "' in array at position " + std::to_string(input.position() + 1));
                        }

                        if (!advance_and_chomp(input)) {
                            throw std::runtime_error("unterminated array starting at position " + std::to_string(start));
                        }
                    }
                }
            }
            input.advance(); // skip the closing bracket.
            break;

        case '{':
            {
                auto ptr = Provisioner_::new_object();
                output.reset(ptr);

                if (!advance_and_chomp(input)) {
                    throw std::runtime_error("unterminated object starting at position " + std::to_string(start));
                }

                if (input.get() != '}') {
                    while (1) {
                        char next = input.get();
                        if (next != '"') {
                            throw std::runtime_error("expected a string as the object key at position " + std::to_string(input.position() + 1));
                        }
                        auto key = extract_string(input);
                        if (ptr->has(key)) {
                            throw std::runtime_error("detected duplicate keys in the object at position " + std::to_string(input.position() + 1));
                        }

                        if (!check_and_chomp(input)) {
                            throw std::runtime_error("unterminated object starting at position " + std::to_string(start));
                        }
                        if (input.get() != ':') {
                            throw std::runtime_error("expected ':' to separate keys and values at position " + std::to_string(input.position() + 1));
                        }

                        if (!advance_and_chomp(input)) {
                            throw std::runtime_error("unterminated object starting at position " + std::to_string(start));
                        }
                        ptr->add(std::move(key), parse_thing<Provisioner_>(input)); // consuming the key here.

                        if (!check_and_chomp(input)) {
                            throw std::runtime_error("unterminated object starting at position " + std::to_string(start));
                        }

                        next = input.get();
                        if (next == '}') {
                            break;
                        } else if (next != ',') {
                            throw std::runtime_error("unknown character '" + std::string(1, next) + "' in array at position " + std::to_string(input.position() + 1));
                        }

                        if (!advance_and_chomp(input)) {
                            throw std::runtime_error("unterminated object starting at position " + std::to_string(start));
                        }
                    }
                }
            }

            input.advance(); // skip the closing brace.
            break;

        case '-':
            if (!input.advance()) {
                throw std::runtime_error("incomplete number starting at position " + std::to_string(start));
            }
            if (!is_digit(input.get())) {
                throw std::runtime_error("invalid number starting at position " + std::to_string(start));
            }
            output.reset(Provisioner_::new_number(-extract_number(input)));
            break;

        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            output.reset(Provisioner_::new_number(extract_number(input)));
            break;

        default:
            throw std::runtime_error(std::string("unknown type starting with '") + std::string(1, current) + "' at position " + std::to_string(start));
    }

    return output;
}

template<class Provisioner_, class Input_>
std::shared_ptr<typename Provisioner_::base> parse_thing_with_chomp(Input_& input) {
    if (!check_and_chomp(input)) {
        throw std::runtime_error("invalid JSON with no contents");
    }
    auto output = parse_thing<Provisioner_>(input);
    if (check_and_chomp(input)) {
        throw std::runtime_error("invalid JSON with trailing non-space characters at position " + std::to_string(input.position() + 1));
    }
    return output;
}
/**
 * @endcond
 */

/**
 * @tparam Input_ Any class that provides the following methods:
 *
 * - `char get() const `, which extracts a `char` from the input source without advancing the position on the byte stream.
 * - `bool valid() const`, to determine whether an input `char` can be `get()` from the input.
 * - `bool advance()`, to advance the input stream and return `valid()` at the new position.
 * - `size_t position() const`, for the current position relative to the start of the byte stream.
 *
 * @param input An instance of an `Input` class, referring to the bytes from a JSON-formatted file or string.
 * @return A pointer to a JSON value.
 */
template<class Input_>
std::shared_ptr<Base> parse(Input_& input) {
    return parse_thing_with_chomp<DefaultProvisioner>(input);
}

/**
 * @tparam Input_ Any class that supplies input characters, see `parse()` for details. 
 *
 * @param input An instance of an `Input` class, referring to the bytes from a JSON-formatted file or string.
 *
 * @return The type of the JSON variable stored in `input`.
 * If the JSON string is invalid, an error is raised.
 */
template<class Input_>
Type validate(Input_& input) {
    auto ptr = parse_thing_with_chomp<FakeProvisioner>(input);
    return ptr->type();
}

/**
 * @cond
 */
class RawReader {
public:
    RawReader(const char* ptr, size_t len) : my_ptr(ptr), my_len(len) {}

private:
    size_t my_pos = 0;
    const char * my_ptr;
    size_t my_len;

public:
    char get() const {
        return my_ptr[my_pos];
    }

    bool valid() const {
        return my_pos < my_len;
    }

    bool advance() {
        ++my_pos;
        return valid();
    }

    size_t position() const {
        return my_pos;
    }
};
/**
 * @endcond
 */

/**
 * @param[in] ptr Pointer to an array containing a JSON string.
 * @param len Length of the array.
 * @return A pointer to a JSON value.
 */
inline std::shared_ptr<Base> parse_string(const char* ptr, size_t len) {
    RawReader input(ptr, len);
    return parse(input);
}

/**
 * @param[in] ptr Pointer to an array containing a JSON string.
 * @param len Length of the array.
 *
 * @return The type of the JSON variable stored in the string.
 * If the JSON string is invalid, an error is raised.
 */
inline Type validate_string(const char* ptr, size_t len) {
    RawReader input(ptr, len);
    return validate(input);
}

/**
 * @cond
 */
class FileReader {
public:
    FileReader(const char* path, size_t buffer_size) : my_handle(std::fopen(path, "rb")), my_buffer(buffer_size) {
        if (!my_handle) {
            throw std::runtime_error("failed to open file at '" + std::string(path) + "'");
        }
        fill();
    }

    ~FileReader() {
        std::fclose(my_handle);
    }

private:
    FILE* my_handle;
    std::vector<char> my_buffer;
    size_t my_available = 0;
    size_t my_index = 0;
    size_t my_overall = 0;
    bool my_finished = false;

public:
    char get() const {
        return my_buffer[my_index];
    }

    bool valid() const {
        return my_index < my_available;
    }

    bool advance() {
        ++my_index;
        if (my_index < my_available) {
            return true;
        }

        my_index = 0;
        my_overall += my_available;
        fill();
        return valid();
    }

    void fill() {
        if (my_finished) {
            my_available = 0;
            return;
        }

        my_available = std::fread(my_buffer.data(), sizeof(char), my_buffer.size(), my_handle);
        if (my_available == my_buffer.size()) {
            return;
        }

        if (std::feof(my_handle)) {
            my_finished = true;
        } else {
            throw std::runtime_error("failed to read file (error " + std::to_string(std::ferror(my_handle)) + ")");
        }
    }

    size_t position() const {
        return my_overall + my_index;
    }
};
/**
 * @endcond
 */

/**
 * @brief Options for `parse_file()` and `validate_file()`.
 */
struct FileReadOptions {
    /**
     * Size of the buffer to use for reading the file.
     */
    size_t buffer_size = 65536;
};

/**
 * @param[in] path Pointer to an array containing a path to a JSON file.
 * @param options Further options.
 * @return A pointer to a JSON value.
 */
inline std::shared_ptr<Base> parse_file(const char* path, const FileReadOptions& options) {
    FileReader input(path, options.buffer_size);
    return parse(input);
}

/**
 * @param[in] path Pointer to an array containing a path to a JSON file.
 * @param options Further options.
 *
 * @return The type of the JSON variable stored in the file.
 * If the JSON file is invalid, an error is raised.
 */
inline Type validate_file(const char* path, const FileReadOptions& options) {
    FileReader input(path, options.buffer_size);
    return validate(input);
}

/**
 * @cond
 */
inline std::shared_ptr<Base> parse_file(const char* path, size_t buffer_size = 65536) {
    FileReadOptions opt;
    opt.buffer_size = buffer_size;
    return parse_file(path, opt);
}

inline Type validate_file(const char* path, size_t buffer_size = 65536) {
    FileReadOptions opt;
    opt.buffer_size = buffer_size;
    return validate_file(path, opt);
}
/**
 * @endcond
 */

}

#endif
