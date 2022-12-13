/**
 * @file weather_archive_test.cpp
 * @date 12/12/2022
 *
 * @brief Unit test for WeatherArchive class 
 */

#include "data/weather_data.h"
#include "data/weather_archive.h"

#include <gtest/gtest.h>

class WeatherArchiveTest : public ::testing::Test {
protected:

    WeatherArchiveTest() {}

    ~WeatherArchiveTest() override {}

    void SetUp() override {}

    void TearDown() override {}

}; // WeatherArchiveTest


/** @brief Test the adding of data to WeatherArchive */
TEST_F(WeatherArchiveTest, AddData) {
    WeatherData newData;
    newData.time = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    newData.maxTemp = 12.3;
    newData.minTemp = 1.34;
    newData.meanTemp = 5.43;
    newData.gas_ppt = 0.134;

    WeatherArchive archive;

    // test adding a new data point 
    archive.addData(newData);
    ASSERT_TRUE(archive.retrieve(newData.time.value()).has_value()) 
        << "WeatherArchive::addData did not add new data";

    // test replacing an existing data point
    WeatherData replaceData;
    replaceData.time = newData.time;
    replaceData.maxTemp = 0.01;
    replaceData.minTemp = 0.02;
    replaceData.meanTemp = 0.03;
    replaceData.gas_ppt = 0.04;

    ASSERT_NE(newData, replaceData) << "Error with AddData test setup, the data "
        << "used to replace the existing data must be different";

    archive.addData(replaceData);
    // use old data's timestamp to reinforce within test that the data was replaced
    auto dataOpt = archive.retrieve(newData.time.value());
    ASSERT_TRUE(dataOpt.has_value()) << "Replacing data within WeatherArchive "
        << "resulted in the data disappearing";
    ASSERT_EQ(replaceData, dataOpt.value()) << "The replaced data does not "
        "match the data retrieved from WeatherArchive";
}

/** @brief Test retrieving data that does not exists within WeatherArchive */
TEST_F(WeatherArchiveTest, RetrieveNoData) {
    const auto startTime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    const auto endTime = startTime + 10;

    WeatherArchive archive;
    ASSERT_FALSE(archive.retrieve(startTime).has_value()) 
        << "WeatherArchive::retrieve was returned data when it should not have";

    ASSERT_TRUE(archive.retrieveRange(startTime, endTime).empty())
        << "WeatherArchive::retrieveRange return data when it should not have";

}

/** @brief Test retrieving data using a range of dates */
TEST_F(WeatherArchiveTest, RetrieveDataRange) {

    // length of the range of data to create and add to WeatherArchive. This value must be a multiple of 2
    // for partsof this test
    const int RangeLength = 10;
    ASSERT_EQ((RangeLength % 2), 0) << "Error in setup of RetrieveDataRange test, RangeLength "
        << "must be a multiple of 2";

    WeatherData newData;
    newData.time = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    newData.maxTemp = 12.3;
    newData.minTemp = 1.34;
    newData.meanTemp = 5.43;
    newData.gas_ppt = 0.134;

    // Save the start and finish of the date range to use with WeatherData::retrieveRange
    const auto startTime = newData.time.value();
    const auto finishTime = newData.time.value() + RangeLength;

    std::vector<WeatherData> addedData; // store the added data for comparison later
    addedData.reserve(RangeLength);

    WeatherArchive archive; // archive object to use for this test

    // Add data to the archive and also save for later
    for (auto i = 0; i < 10; ++i) {
        archive.addData(newData);
        addedData.push_back(newData);
        newData.time = newData.time.value() + 1;
        newData.maxTemp = newData.maxTemp.value() + 1;
        newData.minTemp = newData.minTemp.value() + 1;
        newData.meanTemp = newData.meanTemp.value() + 1;
        newData.gas_ppt = newData.gas_ppt.value() + 1;
    }

    // retrieve data we know should be there
    auto retrieveRangeData = archive.retrieveRange(startTime, finishTime);
    ASSERT_EQ(retrieveRangeData.size(), RangeLength) << 
        "WeatherArchive::retrieveRange returned " << retrieveRangeData.size() <<
        " data points when it should have returned " << RangeLength << " points";

    for (auto i = 0; i < RangeLength; ++i) {
        ASSERT_EQ(retrieveRangeData[i], addedData[i]) 
            << "Retrieved range data does not match the data that was added";
    }

    // retrieve data, where added data is available for only part of the input date range
    retrieveRangeData = archive.retrieveRange(finishTime - (RangeLength / 2), finishTime);
    ASSERT_EQ(retrieveRangeData.size(), (RangeLength / 2)) << 
        "WeatherArchive::retrieveRange returned " << retrieveRangeData.size() <<
        " data points when it should have returned " << (RangeLength / 2) << " points";

    // retrieve data, where added data is within the input date range
    retrieveRangeData = archive.retrieveRange(finishTime + 1, finishTime + 1 + RangeLength);
    ASSERT_TRUE(retrieveRangeData.empty()) << 
        "WeatherArchive::retrieveRange returned " << retrieveRangeData.size() <<
        " data points when it should have not returned any points";
}
