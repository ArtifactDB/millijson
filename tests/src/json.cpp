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
        EXPECT_TRUE(static_cast<millijson::Boolean*>(output.get())->value());
        EXPECT_TRUE(static_cast<const millijson::Boolean*>(output.get())->value()); // check const method
    }

    {
        auto output = parse_raw_json_string("false");
        EXPECT_EQ(output->type(), millijson::BOOLEAN);
        EXPECT_FALSE(static_cast<millijson::Boolean*>(output.get())->value());
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
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value(), "aaron was here");
        EXPECT_EQ(static_cast<const millijson::String*>(output.get())->value(), "aaron was here"); // check const method
    }

    {
        auto output = parse_raw_json_string("\"do\\\"you\\nbelieve\\tin\\rlife\\fafter\\blove\\\\ \\/\"");
        EXPECT_EQ(output->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value(), "do\"you\nbelieve\tin\rlife\fafter\blove\\ /");
    }

    // Unicode shenanigans.
    {
        auto output = parse_raw_json_string("\"I ♥ NATALIE PORTMAN\""); 
        EXPECT_EQ(output->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value(), "I ♥ NATALIE PORTMAN");
    }

    {
        auto output = parse_raw_json_string("\"\\u0041aron\""); // 1 byte UTF-8 
        EXPECT_EQ(output->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value(), "Aaron");
    }

    {
        auto output = parse_raw_json_string("\"sebasti\\u00E8n\""); // 2 byte UTF-8 
        EXPECT_EQ(output->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value(), "sebastièn");

        auto output2 = parse_raw_json_string("\"Fu\\u00dfball\""); // lower case
        EXPECT_EQ(output2->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output2.get())->value(), "Fußball");
    }

    {
        auto output = parse_raw_json_string("\"I \\u2665 NATALIE PORTMAN\""); // 3 byte UTF-8
        EXPECT_EQ(output->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(output.get())->value(), "I ♥ NATALIE PORTMAN");
    }

    parse_raw_json_error(" \"asdasdaasd ", "unterminated string");
    parse_raw_json_error(" \"asdasdaasd\\", "unterminated string");
    parse_raw_json_error(" \"asdasdaasd\\a", "unrecognized escape");
    parse_raw_json_error(" \"asdas\\uasdasd", "invalid unicode");
    parse_raw_json_error(" \"asdas\\u00", "unterminated string");
    parse_raw_json_error(" \"0sdasd\nasdasd\"", "string contains ASCII control character at position 9");
    parse_raw_json_error(" \"sdasd\tasdasd\"", "string contains ASCII control character at position 8");
}

TEST(JsonParsingTest, IntegerLoading) {
    {
        auto output = parse_raw_json_string("1234567890");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 1234567890);
        EXPECT_EQ(static_cast<const millijson::Number*>(output.get())->value(), 1234567890); // check const method.
    }

    {
        auto output = parse_raw_json_string(" 123"); // space in front is correctly parsed.
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 123);
    }

    for (int i = 1; i <= 9; ++i) { // check that we catch all the digits in the switch statement.
        auto output = parse_raw_json_string(std::to_string(i));
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), i);
    }

    // Checking possible terminations.
    {
        auto output = parse_raw_json_string("12345 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 12345);
    }

    {
        auto output = parse_raw_json_string("12345\n");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 12345);
    }

    {
        auto output = parse_raw_json_string("12345\r");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 12345);
    }

    {
        auto output = parse_raw_json_string(" 12345\t");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 12345);
    }

    {
        auto output = parse_raw_json_string("[12345]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        const auto& values = static_cast<millijson::Array*>(output.get())->value();
        EXPECT_EQ(values.size(), 1);
        EXPECT_EQ(values.front()->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(values.front().get())->value(), 12345);
    }

    {
        auto output = parse_raw_json_string("[12345,null]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        const auto& values = static_cast<millijson::Array*>(output.get())->value();
        EXPECT_EQ(values.size(), 2);
        EXPECT_EQ(values.front()->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(values.front().get())->value(), 12345);
    }

    {
        auto output = parse_raw_json_string("{\"a\":12345}");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        const auto& values = static_cast<millijson::Object*>(output.get())->value();
        EXPECT_EQ(values.size(), 1);
        auto it = values.find("a");
        ASSERT_FALSE(it == values.end());
        EXPECT_EQ(it->second->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(it->second.get())->value(), 12345);
    }

    // Works with a negative value.
    {
        auto output = parse_raw_json_string(" -789 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), -789);
    }

    parse_raw_json_error(" 1234L ", "containing 'L'");
    parse_raw_json_error(" 0123456 ", "starting with 0");
    parse_raw_json_error(" 1.", "trailing '.'");
    parse_raw_json_error(" -", "incomplete number");
    parse_raw_json_error(" -a", "invalid number");
}

TEST(JsonParsingTest, FractionLoading) {
    {
        auto output = parse_raw_json_string("123456.7890");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 123456.7890);
    }

    // Works with a zero fractional component.
    {
        auto output = parse_raw_json_string("\t512.00"); // whitespace in front is handled correctly.
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 512);
    }

    // Checking possible terminations.
    {
        auto output = parse_raw_json_string(" 123.456 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 123.456);
    }

    {
        auto output = parse_raw_json_string(" 123.456\n");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 123.456);
    }

    {
        auto output = parse_raw_json_string(" 123.456\r");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 123.456);
    }

    {
        auto output = parse_raw_json_string(" 123.456\t");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 123.456);
    }

    {
        auto output = parse_raw_json_string("[12.345]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        const auto& values = static_cast<millijson::Array*>(output.get())->value();
        EXPECT_EQ(values.size(), 1);
        EXPECT_EQ(values.front()->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(values.front().get())->value(), 12.345);
    }

    {
        auto output = parse_raw_json_string("[12.345,null]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        const auto& values = static_cast<millijson::Array*>(output.get())->value();
        EXPECT_EQ(values.size(), 2);
        EXPECT_EQ(values.front()->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(values.front().get())->value(), 12.345);
    }

    {
        auto output = parse_raw_json_string("{\"a\":12.345}");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        const auto& values = static_cast<millijson::Object*>(output.get())->value();
        EXPECT_EQ(values.size(), 1);
        auto it = values.find("a");
        ASSERT_FALSE(it == values.end());
        EXPECT_EQ(it->second->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(it->second.get())->value(), 12.345);
    }

    // Works with a negative value.
    {
        auto output = parse_raw_json_string(" -0.123456 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), -0.123456);
    }

    parse_raw_json_error(" 1.e2 ", "must be followed");
    parse_raw_json_error(" .12345 ", "starting with '.'");
    parse_raw_json_error(" 12.34f ", "containing 'f'");
}

TEST(JsonParsingTest, ScientificLoading) {
    {
        auto output = parse_raw_json_string(" 1e+2 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 100);
    }

    {
        auto output = parse_raw_json_string(" 1e-2 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 0.01);
    }

    {
        auto output = parse_raw_json_string(" 1e+002 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 100);
    }

    {
        auto output = parse_raw_json_string("9876.5432e+1");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 98765.432);
    }

    // Works with a zero exponent.
    {
        auto output = parse_raw_json_string("\n9e+0"); // whitespace in front is handled correctly.
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 9);
    }

    // Without sign.
    {
        auto output = parse_raw_json_string("2e3");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 2000);
    }

    {
        auto output = parse_raw_json_string("2e002");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 200);
    }

    // Capital E.
    {
        auto output = parse_raw_json_string(" 1.918E+2 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 191.8);
    }

    // Checking termination.
    {
        auto output = parse_raw_json_string(" 123e-1"); // no trailing space.
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 12.3);
    }

    {
        auto output = parse_raw_json_string("123e-1 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 12.3);
    }

    {
        auto output = parse_raw_json_string("123e-1\t");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 12.3);
    }

    {
        auto output = parse_raw_json_string("123e-1\r");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 12.3);
    }

    {
        auto output = parse_raw_json_string("123e-1\n");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(output.get())->value(), 12.3);
    }

    {
        auto output = parse_raw_json_string("[12.3e2]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        const auto& values = static_cast<millijson::Array*>(output.get())->value();
        EXPECT_EQ(values.size(), 1);
        EXPECT_EQ(values.front()->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(values.front().get())->value(), 1230);
    }

    {
        auto output = parse_raw_json_string("[12.3e2,null]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        const auto& values = static_cast<millijson::Array*>(output.get())->value();
        EXPECT_EQ(values.size(), 2);
        EXPECT_EQ(values.front()->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(values.front().get())->value(), 1230);
    }

    {
        auto output = parse_raw_json_string("{\"a\":12.3e2}");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        const auto& values = static_cast<millijson::Object*>(output.get())->value();
        EXPECT_EQ(values.size(), 1);
        auto it = values.find("a");
        ASSERT_FALSE(it == values.end());
        EXPECT_EQ(it->second->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(it->second.get())->value(), 1230);
    }

    parse_raw_json_error(" 1e", "trailing 'e/E'");
    parse_raw_json_error(" 1e ", "'e/E' should be followed");
    parse_raw_json_error(" 1e+", "trailing exponent sign");
    parse_raw_json_error(" 1e+ ", "must be followed by at least one digit");
    parse_raw_json_error(" 1e+1a", "containing 'a'");
}

TEST(JsonParsingTest, ZeroLoading) {
    {
        auto output = parse_raw_json_string("0");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string(" 0");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string("-0");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string("0.000");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string("0E2");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string("0e-2");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string("0 ");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string("0\t");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string("0\r");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string("0\n");
        EXPECT_EQ(output->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(output.get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string("[0]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        const auto& values = static_cast<millijson::Array*>(output.get())->value();
        EXPECT_EQ(values.size(), 1);
        EXPECT_EQ(values.front()->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(values.front().get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string("[0,null]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        const auto& values = static_cast<millijson::Array*>(output.get())->value();
        EXPECT_EQ(values.size(), 2);
        EXPECT_EQ(values.front()->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(values.front().get())->value(), 0);
    }

    {
        auto output = parse_raw_json_string("{\"a\":0}");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        const auto& values = static_cast<millijson::Object*>(output.get())->value();
        EXPECT_EQ(values.size(), 1);
        auto it = values.find("a");
        ASSERT_FALSE(it == values.end());
        EXPECT_EQ(it->second->type(), millijson::NUMBER);
        EXPECT_FLOAT_EQ(static_cast<millijson::Number*>(it->second.get())->value(), 0);
    }

    parse_raw_json_error(" 00.12345 ", "starting with 0");
}

TEST(JsonParsingTest, ArrayLoading) {
    {
        // Check that numbers are correctly terminated by array delimiters.
        auto output = parse_raw_json_string("[100, 200.00, 3.00e+2]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        auto aptr = static_cast<millijson::Array*>(output.get());
        EXPECT_EQ(aptr->value().size(), 3);

        EXPECT_EQ(aptr->value()[0]->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(aptr->value()[0].get())->value(), 100);

        EXPECT_EQ(aptr->value()[1]->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(aptr->value()[1].get())->value(), 200);

        EXPECT_EQ(aptr->value()[2]->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(aptr->value()[2].get())->value(), 300);
    }

    {
        // Throwing some spaces between the structural elements.
        auto output = parse_raw_json_string("[ true , false , null , \"[true, false, null]\" ]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        auto aptr = static_cast<const millijson::Array*>(output.get()); // check const method
        EXPECT_EQ(aptr->value().size(), 4);

        EXPECT_EQ(aptr->value()[0]->type(), millijson::BOOLEAN);
        EXPECT_TRUE(static_cast<const millijson::Boolean*>(aptr->value()[0].get())->value());

        EXPECT_EQ(aptr->value()[1]->type(), millijson::BOOLEAN);
        EXPECT_FALSE(static_cast<const millijson::Boolean*>(aptr->value()[1].get())->value());

        EXPECT_EQ(aptr->value()[2]->type(), millijson::NOTHING);

        EXPECT_EQ(aptr->value()[3]->type(), millijson::STRING);
        EXPECT_EQ(static_cast<const millijson::String*>(aptr->value()[3].get())->value(), "[true, false, null]");
    }

    {
        // No spaces at all.
        auto output = parse_raw_json_string("[null,false,true]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        auto aptr = static_cast<millijson::Array*>(output.get());
        EXPECT_EQ(aptr->value().size(), 3);

        EXPECT_EQ(aptr->value()[0]->type(), millijson::NOTHING);

        EXPECT_EQ(aptr->value()[1]->type(), millijson::BOOLEAN);
        EXPECT_FALSE(static_cast<millijson::Boolean*>(aptr->value()[1].get())->value());

        EXPECT_EQ(aptr->value()[2]->type(), millijson::BOOLEAN);
        EXPECT_TRUE(static_cast<millijson::Boolean*>(aptr->value()[2].get())->value());
    }

    {
        // Empty array.
        auto output = parse_raw_json_string("[]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        auto aptr = static_cast<millijson::Array*>(output.get());
        EXPECT_EQ(aptr->value().size(), 0);

        output = parse_raw_json_string("[   ]");
        EXPECT_EQ(output->type(), millijson::ARRAY);
        aptr = static_cast<millijson::Array*>(output.get());
        EXPECT_EQ(aptr->value().size(), 0);
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
        EXPECT_EQ(aptr->value().size(), 3);

        const auto& foo = aptr->value()["foo"];
        EXPECT_EQ(foo->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(foo.get())->value(), 1);

        const auto& bar = aptr->value()["bar"];
        EXPECT_EQ(bar->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(bar.get())->value(), 2);

        const auto& whee = aptr->value()["whee"];
        EXPECT_EQ(whee->type(), millijson::NUMBER);
        EXPECT_EQ(static_cast<millijson::Number*>(whee.get())->value(), 3);
    }

    {
        // Check that we're robust to spaces.
        auto output = parse_raw_json_string("{ \"foo\" :true , \"bar\": false, \"whee\" : null }");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        auto aptr = static_cast<const millijson::Object*>(output.get()); // check const method.
        EXPECT_EQ(aptr->value().size(), 3);

        const auto& foo = aptr->value().find("foo")->second;
        EXPECT_EQ(foo->type(), millijson::BOOLEAN);
        EXPECT_TRUE(static_cast<const millijson::Boolean*>(foo.get())->value());

        const auto& bar = aptr->value().find("bar")->second;
        EXPECT_EQ(bar->type(), millijson::BOOLEAN);
        EXPECT_FALSE(static_cast<const millijson::Boolean*>(bar.get())->value());

        const auto& whee = aptr->value().find("whee")->second;
        EXPECT_EQ(whee->type(), millijson::NOTHING);
    }

    {
        // No spaces at all.
        auto output = parse_raw_json_string("{\"aaron\":\"lun\",\"jayaram\":\"kancherla\"}");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        auto aptr = static_cast<millijson::Object*>(output.get());
        EXPECT_EQ(aptr->value().size(), 2);

        const auto& foo = aptr->value()["aaron"];
        EXPECT_EQ(foo->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(foo.get())->value(), "lun");

        const auto& bar = aptr->value()["jayaram"];
        EXPECT_EQ(bar->type(), millijson::STRING);
        EXPECT_EQ(static_cast<millijson::String*>(bar.get())->value(), "kancherla");
    }

    {
        // Empty object.
        auto output = parse_raw_json_string("{ }");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        auto aptr = static_cast<millijson::Object*>(output.get());
        EXPECT_EQ(aptr->value().size(), 0);

        output = parse_raw_json_string("{}");
        EXPECT_EQ(output->type(), millijson::OBJECT);
        aptr = static_cast<millijson::Object*>(output.get());
        EXPECT_EQ(aptr->value().size(), 0);
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

millijson::Type validate_raw_json_string(std::string x) {
    return millijson::validate_string(x.c_str(), x.size());
}

TEST(JsonValidateTest, Checks) {
    // Getting some coverage on the validate_* functions.
    EXPECT_EQ(validate_raw_json_string("[ { \"foo\": \"bar\" }, 1e-2, [ null, 98765 ], \"advancer\" ]"), millijson::ARRAY);
    EXPECT_EQ(validate_raw_json_string("false"), millijson::BOOLEAN);
    EXPECT_EQ(validate_raw_json_string("1.323e48"), millijson::NUMBER);
    EXPECT_EQ(validate_raw_json_string("\"ur mum\""), millijson::STRING);
    EXPECT_EQ(validate_raw_json_string("{ \"a\": \"b\" }"), millijson::OBJECT);
    EXPECT_EQ(validate_raw_json_string("null"), millijson::NOTHING);

    EXPECT_ANY_THROW({
        try {
            std::string x = "{";
            millijson::validate_string(x.c_str(), x.size());
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr("unterminated object"));
            throw;
        }
    });

    EXPECT_ANY_THROW({
        try {
            millijson::validate_string(NULL, 0);
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr("no contents"));
            throw;
        }
    });
}
