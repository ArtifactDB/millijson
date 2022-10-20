#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include "millijson/millijson.hpp"

class FileParsingTest : public ::testing::TestWithParam<int> {};

TEST_P(FileParsingTest, ChunkedFile) {
    std::string foo = "[ { \"foo\": \"bar\" }, 1e-2, [ null, 98765 ], \"advancer\" ]";
    {
        std::ofstream output("TEST.json");
        output << foo << std::endl;
    }

    auto output = millijson::parse_file("TEST.json", GetParam());

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

INSTANTIATE_TEST_SUITE_P(
    FileParsing,
    FileParsingTest,
    ::testing::Values(3, 11, 19, 51)
);
