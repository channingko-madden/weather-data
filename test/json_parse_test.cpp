/**
 * @file json_parse_test.cpp
 * @date 12/8/2022
 *
 * @brief Unit test for json weather data payload parsing
 */

#include "json_parse.h"
#include "data/weather_data.h"

#include <gtest/gtest.h>
#include <string>
#include <cmath>

/**
 * @class PayloadParserTest json_parse_test.cpp "test/json_parse_test.cpp"
 * @brief This class tests the parsing of json weather data
 */
class PayloadParserTest : public ::testing::Test {
protected:

    PayloadParserTest() {}

    ~PayloadParserTest() override {}

    void SetUp() override {}

    void TearDown() override {}

}; // PayloadParserTest

/** @brief Test the parsing of valid weather data */
TEST_F(PayloadParserTest, ParseValidWeatherData) {
    const std::string validWeatherData{
        "{\"date\": \"2016-03-03\","
        "\"tmax\": 28.758,"
        "\"tmin\": 3.896,"
        "\"tmean\": 16.327,"
        "\"ppt\": 0.0}"};

    EXPECT_NO_THROW({
        const auto weatherData = jsonparse::parseWeather(jsonparse::json_from_string(validWeatherData));
        ASSERT_FLOAT_EQ(weatherData.maxTemp.value(), 28.758f);
        ASSERT_FLOAT_EQ(weatherData.minTemp.value(), 3.896f);
        ASSERT_FLOAT_EQ(weatherData.meanTemp.value(), 16.327f);
        ASSERT_FLOAT_EQ(weatherData.gas_ppt.value(), 0.0f);
    }) << "Exception thrown parsing valid weather data";
}

/** @brief Test the parsing of weather data that has extra whitespace in the date */
TEST_F(PayloadParserTest, ParseInvalidDate) {
    const std::string invalidDate{
        "{\"date\": \" 2016-03-03 \","
        "\"tmax\": 28.758,"
        "\"tmin\": 3.896,"
        "\"tmean\": 16.327,"
        "\"ppt\": 0.0}"};

    EXPECT_NO_THROW({
        const auto weatherData = jsonparse::parseWeather(jsonparse::json_from_string(invalidDate));
        ASSERT_TRUE(weatherData.time.has_value()) << "time should be set";
        ASSERT_FLOAT_EQ(weatherData.maxTemp.value(), 28.758f);
        ASSERT_FLOAT_EQ(weatherData.minTemp.value(), 3.896f);
        ASSERT_FLOAT_EQ(weatherData.meanTemp.value(), 16.327f);
        ASSERT_FLOAT_EQ(weatherData.gas_ppt.value(), 0.0f);
    }) << "Exception thrown parsing valid weather data where data has extra whitespace";

    const std::string noDate{
        "{\"tmax\": 28.758,"
        "\"tmin\": 3.896,"
        "\"tmean\": 16.327,"
        "\"ppt\": 0.0}"};

    EXPECT_NO_THROW( {
            const auto weatherData = jsonparse::parseWeather(jsonparse::json_from_string(noDate)); 
            ASSERT_FALSE(weatherData.time.has_value()) << "time should not be set";
            ASSERT_FLOAT_EQ(weatherData.maxTemp.value(), 28.758f);
            ASSERT_FLOAT_EQ(weatherData.minTemp.value(), 3.896f);
            ASSERT_FLOAT_EQ(weatherData.meanTemp.value(), 16.327f);
            ASSERT_FLOAT_EQ(weatherData.gas_ppt.value(), 0.0f);
        })<< "Exception should not be thrown because of missing date";
}

/** @brief Test the parsing of weather data that is missing numeral data */
TEST_F(PayloadParserTest, ParseNoNumeralData) {
    const std::string noTmaxData{"{\"date\": \"2016-03-03\"}"};

    EXPECT_NO_THROW({
        const auto weatherData = jsonparse::parseWeather(jsonparse::json_from_string(noTmaxData));
        ASSERT_TRUE(weatherData.time.has_value()) << "time should be set";
        ASSERT_FALSE(weatherData.maxTemp.has_value()) << "maxTemp should not be set";
        ASSERT_FALSE(weatherData.minTemp.has_value()) << "minTemp should not be set";
        ASSERT_FALSE(weatherData.meanTemp.has_value()) << "meanTemp should not be set";
        ASSERT_FALSE(weatherData.gas_ppt.has_value()) << "gas_ppt should not be set";
    }) << "Exception should not be thrown if missing numeral data";
}
