#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include "millijson/millijson.hpp"
#include "byteme/byteme.hpp"

class FileParsingTest : public ::testing::TestWithParam<int> {};

TEST_P(FileParsingTest, ChunkedFile) {
    std::string foo = "[ { \"foo\": \"bar\" }, 1e-2, [ null, 98765 ], \"advancer\" ]";
    {
        std::ofstream output("TEST.json");
        output << foo << std::endl;
    }

    millijson::FileReadOptions opt;
    opt.buffer_size = GetParam();
    auto output = millijson::parse_file("TEST.json", opt);
    millijson::validate_file("TEST.json", opt);

    EXPECT_EQ(output->type(), millijson::ARRAY);
    const auto& array = static_cast<const millijson::Array*>(output.get())->value();
    EXPECT_EQ(array.size(), 4);

    // Checking the first object.
    EXPECT_EQ(array[0]->type(), millijson::OBJECT);
    const auto& mapping = static_cast<const millijson::Object*>(array[0].get())->value();
    EXPECT_EQ(mapping.size(), 1);

    auto it = mapping.find("foo");
    EXPECT_TRUE(it != mapping.end());
    EXPECT_EQ((it->second)->type(), millijson::STRING);
    EXPECT_EQ(static_cast<const millijson::String*>((it->second).get())->value(), "bar");

    // Checking the second number.
    EXPECT_EQ(array[1]->type(), millijson::NUMBER);
    EXPECT_EQ(static_cast<const millijson::Number*>(array[1].get())->value(), 0.01);

    // Checking the third array.
    EXPECT_EQ(array[2]->type(), millijson::ARRAY);
    const auto& array2 = static_cast<const millijson::Array*>(array[2].get())->value();
    EXPECT_EQ(array2.size(), 2);
    EXPECT_EQ(array2[0]->type(), millijson::NOTHING);
    EXPECT_EQ(array2[1]->type(), millijson::NUMBER);
    EXPECT_EQ(static_cast<const millijson::Number*>(array2[1].get())->value(), 98765);

    // Checking the fourth.
    EXPECT_EQ(array[3]->type(), millijson::STRING);
    EXPECT_EQ(static_cast<const millijson::String*>(array[3].get())->value(), "advancer");
}

TEST_P(FileParsingTest, BytemeCompatibility) {
    std::string foo = "{ \"foo\": \"bar\", \"YAY\": [ 5, 3, 2 ], \"whee\": null }";
    {
        std::ofstream output("TEST.json");
        output << foo << std::endl;
    }

    byteme::PerByteSerial<char> pb(
        std::make_unique<byteme::RawFileReader>(
            "TEST.json",
            [&]{
                byteme::RawFileReaderOptions opt;
                opt.buffer_size = GetParam();
                return opt;
            }()
        )
    );
    auto output = millijson::parse(pb);

    EXPECT_EQ(output->type(), millijson::OBJECT);
    const auto& mapping = static_cast<const millijson::Object*>(output.get())->value();
    EXPECT_EQ(mapping.size(), 3);

    // Checking the first key.
    auto it = mapping.find("foo");
    EXPECT_TRUE(it != mapping.end());
    EXPECT_EQ((it->second)->type(), millijson::STRING);
    EXPECT_EQ(static_cast<const millijson::String*>((it->second).get())->value(), "bar");

    // Checking the second key.
    it = mapping.find("YAY");
    EXPECT_TRUE(it != mapping.end());
    EXPECT_EQ((it->second)->type(), millijson::ARRAY);

    const auto& arr = static_cast<const millijson::Array*>((it->second).get())->value();
    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0]->type(), millijson::NUMBER);
    EXPECT_EQ(arr[1]->type(), millijson::NUMBER);
    EXPECT_EQ(arr[2]->type(), millijson::NUMBER);

    // Checking the third key.
    it = mapping.find("whee");
    EXPECT_TRUE(it != mapping.end());
    EXPECT_EQ((it->second)->type(), millijson::NOTHING);
}

INSTANTIATE_TEST_SUITE_P(
    FileParsing,
    FileParsingTest,
    ::testing::Values(3, 11, 19, 51)
);

TEST(FileParsing, Errors) {
    EXPECT_ANY_THROW({
        try {
            millijson::parse_file("TEST-missing.json", {});
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr("failed to open file"));
            throw;
        }
    });
}

TEST(FileParsing, CheckBufferSize) {
    EXPECT_EQ(millijson::FileReader::check_buffer_size(1), 1);
    constexpr auto maxed = std::numeric_limits<std::size_t>::max();
    EXPECT_LE(millijson::FileReader::check_buffer_size(maxed), maxed);
}
