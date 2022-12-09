/**
 * @file payload_parser_test.cpp
 * @date 12/8/2022
 *
 * @brief Unit test for json weather data payload parsing
 */

#include "payload_parser.h"

#include <gtest/gtest.h>
#include <string>
#include <cmath>

/**
 * @class PayloadParserTest payload_parser_test.cpp "test/payload_parser_test.cpp"
 * @brief This class tests the parsing of json weather data
 */
class PayloadParserTest : public ::testing::Test {
protected:

    PayloadParserTest() {}

    ~PayloadParserTest() override {}

    void SetUp() override {}

    void TearDown() override {}

};

/** @brief Test the parsing of valid weather data */
TEST_F(PayloadParserTest, ParseValidWeatherData) {
    const std::string validWeatherData{
        "{\"date\": \"2016-03-03\","
        "\"tmax\": 28.758,"
        "\"tmin\": 3.896,"
        "\"tmean\": 16.327,"
        "\"ppt\": 0.0}"};

    EXPECT_NO_THROW({
        const auto weatherData = payloadparser::parseWeather(payloadparser::json_from_string(validWeatherData));
        ASSERT_FLOAT_EQ(weatherData.maxTemp, 28.758f);
        ASSERT_FLOAT_EQ(weatherData.minTemp, 3.896f);
        ASSERT_FLOAT_EQ(weatherData.meanTemp, 16.327f);
        ASSERT_FLOAT_EQ(weatherData.gas_ppt, 0.0f);
    }) << "Exception thrown parsing valid weather data";
}

/** @brief Test the parsing of weather data that has an invalid or missing date */
TEST_F(PayloadParserTest, ParseInvalidDate) {
    const std::string invalidDate{
        "{\"date\": \"20-03-03\","
        "\"tmax\": 28.758,"
        "\"tmin\": 3.896,"
        "\"tmean\": 16.327,"
        "\"ppt\": 0.0}"};

    EXPECT_THROW(
        {const auto weatherData = payloadparser::parseWeather(payloadparser::json_from_string(invalidDate)); }, 
        payloadparser::IncorrectJson) << "Expected an IncorrectJson to be thrown due to invalid date";

    const std::string noDate{
        "{\"tmax\": 28.758,"
        "\"tmin\": 3.896,"
        "\"tmean\": 16.327,"
        "\"ppt\": 0.0}"};

    EXPECT_THROW(
        {const auto weatherData = payloadparser::parseWeather(payloadparser::json_from_string(noDate)); }, 
        payloadparser::IncorrectJson) << "Expected an IncorrectJson to be thrown due to missing date";
}

/** @brief Test the parsing of weather data that is missing numeral data */
TEST_F(PayloadParserTest, ParseNoNumeralData) {
    const std::string noTmaxData{"{\"date\": \"2016-03-03\"}"};

    EXPECT_NO_THROW({
        const auto weatherData = payloadparser::parseWeather(payloadparser::json_from_string(noTmaxData));
        ASSERT_TRUE(std::isnan(weatherData.maxTemp)) << "maxTemp should be nan";
        ASSERT_TRUE(std::isnan(weatherData.minTemp)) << "minTemp should be nan";
        ASSERT_TRUE(std::isnan(weatherData.meanTemp)) << "meanTemp should be nan";
        ASSERT_TRUE(std::isnan(weatherData.gas_ppt)) << "gas_ppt should be nan";
    }) << "Exception should not be thrown if missing numeral data";
}
