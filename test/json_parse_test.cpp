/**
 * @file json_parse_test.cpp
 * @date 12/12/2022
 *
 * @brief Unit test for json weather data payload parsing functions within the jsonparse namespace
 */

#include "json_parse.h"
#include "data/weather_data.h"

#include <gtest/gtest.h>
#include <string>
#include <cmath>
#include <chrono>

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
        const auto weatherData = jsonparse::parseWeather(jsonparse::jsonFromString(validWeatherData));
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
        const auto weatherData = jsonparse::parseWeather(jsonparse::jsonFromString(invalidDate));
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
            const auto weatherData = jsonparse::parseWeather(jsonparse::jsonFromString(noDate)); 
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
        const auto weatherData = jsonparse::parseWeather(jsonparse::jsonFromString(noTmaxData));
        ASSERT_TRUE(weatherData.time.has_value()) << "time should be set";
        ASSERT_FALSE(weatherData.maxTemp.has_value()) << "maxTemp should not be set";
        ASSERT_FALSE(weatherData.minTemp.has_value()) << "minTemp should not be set";
        ASSERT_FALSE(weatherData.meanTemp.has_value()) << "meanTemp should not be set";
        ASSERT_FALSE(weatherData.gas_ppt.has_value()) << "gas_ppt should not be set";
    }) << "Exception should not be thrown if missing numeral data";
}

/** 
 * @brief Test the conversion of a date string YYYY-MM-DD to Unix time
 * by converting it, then converting it back
 */
TEST_F(PayloadParserTest, DateToUnixConversion) {
    const std::string dateString{"2016-03-04"};

    const auto unixOpt = jsonparse::dateToUnix(dateString);
    ASSERT_TRUE(unixOpt.has_value()) << "Date string to Unix conversion failed";
    ASSERT_EQ(dateString, jsonparse::unixToDate(unixOpt.value())) 
        << "Date string -> Unix time -> Date string conversion failed";
}

/** @brief Test creating a JSON schema from a WeatherData object */
TEST_F(PayloadParserTest, CreateWeatherJson) {
    WeatherData data;
    data.time = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    data.maxTemp = 12.345;
    data.minTemp = 34.567;
    data.meanTemp  = 23.456;
    data.gas_ppt = 5.678;

    const auto schema = jsonparse::createWeatherJson(data);

    ASSERT_TRUE(schema.isObject());
    ASSERT_TRUE(schema.isMember(jsonparse::DATE_KEY) 
            && schema[jsonparse::DATE_KEY].isString()) << jsonparse::DATE_KEY 
        << " key was not created correctly in conversion of WeatherData to JSON";
    ASSERT_EQ(jsonparse::unixToDate(data.time.value()),
            schema[jsonparse::DATE_KEY].asString()) << jsonparse::DATE_KEY
        << " key's value was not set correctly";

    ASSERT_TRUE(schema.isMember(jsonparse::TMAX_KEY)
            && schema[jsonparse::TMAX_KEY].isNumeric()) << jsonparse::TMAX_KEY
        << " key was not created correctly in conversion of WeatherData to JSON";
    ASSERT_FLOAT_EQ(schema[jsonparse::TMAX_KEY].asFloat(), data.maxTemp.value())
        << jsonparse::TMAX_KEY << " key's value was not set correctly";

    ASSERT_TRUE(schema.isMember(jsonparse::TMIN_KEY)
            && schema[jsonparse::TMIN_KEY].isNumeric()) << jsonparse::TMIN_KEY
        << " key was not created correctly in conversion of WeatherData to JSON";
    ASSERT_FLOAT_EQ(schema[jsonparse::TMIN_KEY].asFloat(), data.minTemp.value())
        << jsonparse::TMIN_KEY << " key's value was not set correctly";

    ASSERT_TRUE(schema.isMember(jsonparse::TMEAN_KEY)
            && schema[jsonparse::TMEAN_KEY].isNumeric()) << jsonparse::TMEAN_KEY
        << " key was not created correctly in conversion of WeatherData to JSON";
    ASSERT_FLOAT_EQ(schema[jsonparse::TMEAN_KEY].asFloat(), data.meanTemp.value())
        << jsonparse::TMEAN_KEY << " key's value was not set correctly";

    ASSERT_TRUE(schema.isMember(jsonparse::PPT_KEY)
            && schema[jsonparse::PPT_KEY].isNumeric()) << jsonparse::PPT_KEY
        << " key was not created correctly in conversion of WeatherData to JSON";
    ASSERT_FLOAT_EQ(schema[jsonparse::PPT_KEY].asFloat(), data.gas_ppt.value())
        << jsonparse::PPT_KEY << " key's value was not set correctly";
}
