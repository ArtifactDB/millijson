#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "millijson/millijson.hpp"

std::shared_ptr<millijson::Base> parse_raw_json_string(std::string x) {
    return millijson::parse_string(x.c_str(), x.size());
}

void parse_raw_json_error(std::string x, std::string msg) {
    EXPECT_ANY_THROW({
        try {
            parse_raw_json_string(x);
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
            throw;
        }
    });
}

TEST(JsonParsingTest, NullLoading) {
    {
        auto output = parse_raw_json_string("null");
        EXPECT_EQ(output->type(), millijson::NOTHING);
    }

    parse_raw_json_error("none", "expected a 'null'");
    parse_raw_json_error("nully", "trailing");
    parse_raw_json_error("nul", "expected a 'null'");
}

TEST(JsonParsingTest, BooleanLoading) {
    {
        auto output = parse_raw_json_string("true");
        EXPECT_EQ(output->type(), millijson::BOOLEAN);
        EXPECT_TRUE(static_cast<millijson::Boolean*>(output.get())->value);
    }

    {
        auto output = parse_raw_json_string("false");
        EXPECT_EQ(output->type(), millijson::BOOLEAN);
        EXPECT_FALSE(static_cast<millijson::Boolean*>(output.get())->value);
    }

    parse_raw_json_error("fals", "expected a 'false'");
    parse_raw_json_error("falsy", "expected a 'false'");
    parse_raw_json_error("falsey", "trailing");

    parse_raw_json_error("tru", "expected a 'true'");
    parse_raw_json_error("truthy", "expected a 'true'");
    parse_raw_json_error("true-ish", "trailing");
}

TEST(JsonParsingTest, StringLoading) {
    {
        auto output = parse_raw_json_string("\t\n \"aaron was here\" ");
        EXPECT_EQ(output->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value, "aaron was here");
    }

    {
        auto output = parse_raw_json_string("\"do\\\"you\\nbelieve\\tin\\rlife\\fafter\\blove\\\\ \\/\"");
        EXPECT_EQ(output->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value, "do\"you\nbelieve\tin\rlife\fafter\blove\\ /");
    }

    // Unicode shenanigans.
    {
        auto output = parse_raw_json_string("\"I ♥ NATALIE PORTMAN\""); 
        EXPECT_EQ(output->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value, "I ♥ NATALIE PORTMAN");
    }

    {
        auto output = parse_raw_json_string("\"\\u0041aron\""); // 1 byte UTF-8 
        EXPECT_EQ(output->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value, "Aaron");
    }

    {
        auto output = parse_raw_json_string("\"sebasti\\u00E8n\""); // 2 byte UTF-8 
        EXPECT_EQ(output->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value, "sebastièn");

        auto output2 = parse_raw_json_string("\"Fu\\u00dfball\""); // lower case
        EXPECT_EQ(output2->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output2.get())->value, "Fußball");
    }

    {
        auto output = parse_raw_json_string("\"I \\u2665 NATALIE PORTMAN\""); // 3 byte UTF-8
        EXPECT_EQ(output->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value, "I ♥ NATALIE PORTMAN");
    }

    parse_raw_json_error(" \"asdasdaasd ", "unterminated string");
    parse_raw_json_error(" \"asdasdaasd\\", "unterminated string");
    parse_raw_json_error(" \"asdasdaasd\\a", "unrecognized escape");
    parse_raw_json_error(" \"asdas\\uasdasd", "invalid unicode");
    parse_raw_json_error(" \"asdas\\u00", "unterminated string");
}

TEST(JsonParsingTest, IntegerLoading) {
    {
        auto output = parse_raw_json_string(" 12345 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 12345);
    }

    {
        auto output = parse_raw_json_string(" 123"); // no trailing space.
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 123);
    }

    {
        auto output = parse_raw_json_string(" 0 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 0);
    }

    {
        auto output = parse_raw_json_string(" 0"); // no trailing space.
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 0);
    }

    {
        auto output = parse_raw_json_string(" -789 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, -789);
    }

    parse_raw_json_error(" 1234L ", "containing 'L'");
    parse_raw_json_error(" 0123456 ", "starting with 0");
    parse_raw_json_error(" 1.", "trailing '.'");
    parse_raw_json_error(" -", "incomplete number");
}

TEST(JsonParsingTest, FractionLoading) {
    {
        auto output = parse_raw_json_string(" 1.2345 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 1.2345);
    }

    {
        auto output = parse_raw_json_string(" 123.456 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 123.456);
    }

    {
        auto output = parse_raw_json_string(" 512.00"); // no trailing space.
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 512);
    }

    {
        auto output = parse_raw_json_string(" -0.123456 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value, -0.123456);
    }

    parse_raw_json_error(" 1.e2 ", "must be followed");
    parse_raw_json_error(" 00.12345 ", "starting with 0");
    parse_raw_json_error(" .12345 ", "starting with '.'");
    parse_raw_json_error(" 12.34f ", "containing 'f'");
}

TEST(JsonParsingTest, ScientificLoading) {
    {
        auto output = parse_raw_json_string(" 1e+2 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 100);
    }

    {
        auto output = parse_raw_json_string(" 1e+002 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 100);
    }

    {
        auto output = parse_raw_json_string("2e002");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 200);
    }

    {
        auto output = parse_raw_json_string(" 1e-2 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 0.01);
    }

    {
        auto output = parse_raw_json_string(" 0E2 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 0);
    }

    {
        auto output = parse_raw_json_string(" 123e-1"); // no trailing space.
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value, 12.3);
    }

    {
        auto output = parse_raw_json_string(" 1.918E+2 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value, 191.8);
    }

    parse_raw_json_error(" 1e", "trailing 'e/E'");
    parse_raw_json_error(" 1e ", "'e/E' should be followed");
    parse_raw_json_error(" 1e+", "trailing exponent sign");
    parse_raw_json_error(" 1e+ ", "must be followed by at least one digit");
    parse_raw_json_error(" 1e+1a", "containing 'a'");
}

TEST(JsonParsingTest, ArrayLoading) {
    {
        // Check that numbers are correctly terminated by array delimiters.
        auto output = parse_raw_json_string("[100, 200.00, 3.00e+2]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        auto aptr = static_cast<millijson::Array*>(output.get());
        EXPECT_EQ(aptr->values.size(), 3);

        EXPECT_EQ(aptr->values[0]->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(aptr->values[0].get())->value, 100);

        EXPECT_EQ(aptr->values[1]->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(aptr->values[1].get())->value, 200);

        EXPECT_EQ(aptr->values[2]->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(aptr->values[2].get())->value, 300);
    }

    {
        // Throwing some spaces between the structural elements.
        auto output = parse_raw_json_string("[ true , false , null , \"[true, false, null]\" ]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        auto aptr = static_cast<millijson::Array*>(output.get());
        EXPECT_EQ(aptr->values.size(), 4);

        EXPECT_EQ(aptr->values[0]->type(), millijson::BOOLEAN);
        EXPECT_TRUE(static_cast<millijson::Boolean*>(aptr->values[0].get())->value);

        EXPECT_EQ(aptr->values[1]->type(), millijson::BOOLEAN);
        EXPECT_FALSE(static_cast<millijson::Boolean*>(aptr->values[1].get())->value);

        EXPECT_EQ(aptr->values[2]->type(), millijson::NOTHING);

        EXPECT_EQ(aptr->values[3]->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(aptr->values[3].get())->value, "[true, false, null]");
    }

    {
        // No spaces at all.
        auto output = parse_raw_json_string("[null,false,true]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        auto aptr = static_cast<millijson::Array*>(output.get());
        EXPECT_EQ(aptr->values.size(), 3);

        EXPECT_EQ(aptr->values[0]->type(), millijson::NOTHING);

        EXPECT_EQ(aptr->values[1]->type(), millijson::BOOLEAN);
        EXPECT_FALSE(static_cast<millijson::Boolean*>(aptr->values[1].get())->value);

        EXPECT_EQ(aptr->values[2]->type(), millijson::BOOLEAN);
        EXPECT_TRUE(static_cast<millijson::Boolean*>(aptr->values[2].get())->value);
    }

    {
        // Empty array.
        auto output = parse_raw_json_string("[]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        auto aptr = static_cast<millijson::Array*>(output.get());
        EXPECT_EQ(aptr->values.size(), 0);

        output = parse_raw_json_string("[   ]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        aptr = static_cast<millijson::Array*>(output.get());
        EXPECT_EQ(aptr->values.size(), 0);
    }

    parse_raw_json_error(" [", "unterminated array");
    parse_raw_json_error(" [ 1,", "unterminated array");
    parse_raw_json_error(" [ 1 ", "unterminated array");
    parse_raw_json_error(" [ 1, ", "unterminated array");
    parse_raw_json_error(" [ 1, ]", "unknown type starting with ']'");
    parse_raw_json_error(" [ 1 1 ]", "unknown character '1'");
    parse_raw_json_error(" [ , ]", "unknown type starting with ','");
}

TEST(JsonParsingTest, ObjectLoading) {
    {
        // Check that numbers are correctly terminated by object delimiters.
        auto output = parse_raw_json_string("{\"foo\": 1, \"bar\":2, \"whee\":3}");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        auto aptr = static_cast<millijson::Object*>(output.get());
        EXPECT_EQ(aptr->values.size(), 3);

        const auto& foo = aptr->values["foo"];
        EXPECT_EQ(foo->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(foo.get())->value, 1);

        const auto& bar = aptr->values["bar"];
        EXPECT_EQ(bar->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(bar.get())->value, 2);

        const auto& whee = aptr->values["whee"];
        EXPECT_EQ(whee->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(whee.get())->value, 3);
    }

    {
        // Check that we're robust to spaces.
        auto output = parse_raw_json_string("{ \"foo\" :true , \"bar\": false, \"whee\" : null }");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        auto aptr = static_cast<millijson::Object*>(output.get());
        EXPECT_EQ(aptr->values.size(), 3);

        const auto& foo = aptr->values["foo"];
        EXPECT_EQ(foo->type(), millijson::BOOLEAN);
        EXPECT_TRUE(static_cast<millijson::Boolean*>(foo.get())->value);

        const auto& bar = aptr->values["bar"];
        EXPECT_EQ(bar->type(), millijson::BOOLEAN);
        EXPECT_FALSE(static_cast<millijson::Boolean*>(bar.get())->value);

        const auto& whee = aptr->values["whee"];
        EXPECT_EQ(whee->type(), millijson::NOTHING);
    }

    {
        // No spaces at all.
        auto output = parse_raw_json_string("{\"aaron\":\"lun\",\"jayaram\":\"kancherla\"}");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        auto aptr = static_cast<millijson::Object*>(output.get());
        EXPECT_EQ(aptr->values.size(), 2);

        const auto& foo = aptr->values["aaron"];
        EXPECT_EQ(foo->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(foo.get())->value, "lun");

        const auto& bar = aptr->values["jayaram"];
        EXPECT_EQ(bar->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(bar.get())->value, "kancherla");
    }

    {
        // Empty object.
        auto output = parse_raw_json_string("{ }");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        auto aptr = static_cast<millijson::Object*>(output.get());
        EXPECT_EQ(aptr->values.size(), 0);

        output = parse_raw_json_string("{}");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        aptr = static_cast<millijson::Object*>(output.get());
        EXPECT_EQ(aptr->values.size(), 0);
    }

    parse_raw_json_error(" {", "unterminated object");
    parse_raw_json_error(" { \"foo\"", "unterminated object");
    parse_raw_json_error(" { \"foo\" :", "unterminated object");
    parse_raw_json_error(" { \"foo\" : \"bar\"", "unterminated object");
    parse_raw_json_error(" { \"foo\" : \"bar\", ", "unterminated object");
    parse_raw_json_error(" { true", "expected a string");
    parse_raw_json_error(" { \"foo\" , \"bar\" }", "expected ':'");
    parse_raw_json_error(" { \"foo\": \"bar\", }", "expected a string");
    parse_raw_json_error(" { \"foo\": \"bar\": \"stuff\" }", "unknown character ':'");
    parse_raw_json_error(" { \"foo\": \"bar\", \"foo\": \"stuff\" }", "duplicate");
}

TEST(JsonParsingTest, QuickGet) {
    std::string foo = "[ { \"foo\": \"bar\" }, 1e-2, [ null, 98765 ], \"advancer\" ]";
    auto output = parse_raw_json_string(foo);

    EXPECT_EQ(output->type(), millijson::ARRAY);
    const auto& array = output->get_array();
    EXPECT_EQ(array.size(), 4);

    // Checking the first object.
    EXPECT_EQ(array[0]->type(), millijson::OBJECT);
    const auto& mapping = array[0]->get_object();
    EXPECT_EQ(mapping.size(), 1);

    auto it = mapping.find("foo");
    EXPECT_TRUE(it != mapping.end());
    EXPECT_EQ((it->second)->type(), millijson::STRING);
    EXPECT_EQ((it->second)->get_string(), "bar");

    // Checking the second number.
    EXPECT_EQ(array[1]->type(), millijson::NUMBER);
    EXPECT_EQ(array[1]->get_number(), 0.01);

    // Checking the third array.
    EXPECT_EQ(array[2]->type(), millijson::ARRAY);
    const auto& array2 = array[2]->get_array();
    EXPECT_EQ(array2.size(), 2);
    EXPECT_EQ(array2[0]->type(), millijson::NOTHING);
    EXPECT_EQ(array2[1]->type(), millijson::NUMBER);
    EXPECT_EQ(array2[1]->get_number(), 98765);

    // Checking the fourth.
    EXPECT_EQ(array[3]->type(), millijson::STRING);
    EXPECT_EQ(array[3]->get_string(), "advancer");
}

TEST(JsonValidateTest, Checks) {
    // Getting some coverage on the validate_* functions.
    std::string x = "[ { \"foo\": \"bar\" }, 1e-2, [ null, 98765 ], \"advancer\" ]";
    millijson::validate_string(x.c_str(), x.size());

    x = "{";
    EXPECT_ANY_THROW({
        try {
            millijson::validate_string(x.c_str(), x.size());
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr("unterminated object"));
            throw;
        }
    });
}
