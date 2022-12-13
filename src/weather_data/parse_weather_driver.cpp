/**
 * @file parse_weather_driver.cpp
 * @date 12/12/2022
 *
 * @brief The driver class for the parseweather cli script
 */

#include "parse_weather_driver.h"
#include "json_parse.h"

#include "jsoncpp/json/value.h"
#include "date/date.h"
#include <algorithm>
#include <regex>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <random>
#include <chrono>

void ParseWeatherDriver::setOptions(CLI::App& app) {
    // json input file is required. Use CLI to check that the file exists
    mpFileOption = app.add_option(
            "-f, --file",
            mInputFilename,
            "Absolute path to json weather data file.\n"
            "Ex: parseweather -f /home/path/to/file.json")
        ->required()
        ->check(CLI::ExistingFile);
                
    // date option
    mpDateOption = app.add_option(
            "-d, --date",
            mOptionSingleString,
            "A specific day to retrieve weather data for, "
            "formatted as YYYY-MM-DD\nEx: -d 2022-01-01")
        ->check([](const std::string& str) {
                std::smatch match;
                if (std::regex_match(str, match, jsonparse::dateRegex)) {
                    return std::string();
                } else {
                    throw CLI::ValidationError(
                            "DateOptionError",
                            "Incorrect input for -d, --date option");
                }
            });

    // range option
    mpRangeOption = app.add_option(
            "-r, --range",
            mOptionSingleString,
            "Return weather data from the specific time range as a JSON Array."
            " If data within the range is missing, all present data will be output.\n"
            " The input must be formatted as YYYY-MM-DD|YYYY-MM-DD\n"
            "Ex: -r 2022-01-01|2022-12-31\n"
            "\t(note: In the terminal, the | character will be interpreted as the "
            "pipe command. Therefore it needs to be escaped.\n"
            "\tex: 2022-01-01\\|2022-12-31)")
        ->excludes(mpDateOption) // exludes so that only one option is accepted at a time
        ->check([this](const std::string& str) {
            if (checkDateRange(str)) {
                return std::string();
            } else {
                throw CLI::ValidationError("RangeOptionError", "Incorrect input for -r, --range option");
            }
        });

    // mean option, validity is easier checked with the parsed contents
    mpMeanOption = app.add_option(
            "-m, --mean",
            mOptionMultiString,
            "Return the mean of the variable provided over the specific time range.\n"
            "If data within the range is missing, only present data will be used for calculating the mean.\n"
            "The date range must be formatted as YYYY-MM-DD|YYYY-MM-DD."
            "\nPossible variable options are: tmax, tmin, tmean, and ppt."
            "\nEx: -m 2022-01-01|2022-12-31 tmax  or -m tmax 2022-01-01|2022-12-31")
        ->expected(2)
        ->excludes(mpDateOption) // excludes so that only one option is accepted at a time
        ->excludes(mpRangeOption);

    // historical sample option, validity is easier checked with the parsed contents
    mpSampleHistoryOption = app.add_option(
            "-s, --sample-history",
            mOptionMultiString,
            "Return weather data similar to the --range option, except that for each date "
            "within the range, randomly select a year from the second option,\n"
            "and return the data from the same day of the randomly chosen year.\n"
            "If there is no available data for a given date to sample from, it is ommitted from"
            "the returned data."
            "\nThe date range must be formatted as YYYY-MM-DD|YYYY-MM-DD"
            "\nThe year range must be formatted as YYYY|YYYY"
            "\nEx: -s 2022-01-01|2022-12-31 2018|2022 or -s 2018|2022 2022-01-01|2022-12-31")
        ->expected(2)
        ->excludes(mpDateOption) // excludes so that only one option is accepted at a time
        ->excludes(mpRangeOption)
        ->excludes(mpMeanOption);
}

void ParseWeatherDriver::run(CLI::App& app) {
    // This should always be true since this is required, but be safe and check
    if (mpFileOption && mpFileOption->count()) { 
        if (!readInputFile()) { // error messages are output within this function
            return;
        }
    } else {
        throw CLI::ValidationError(
                "FileOptionError",
                "An error occurred, the required --file option was not passed\n");
    }

    // run options!
    if (mpDateOption && mpDateOption->count()) {
        runDateOption();
    } else if (mpRangeOption && mpRangeOption->count()) {
        runRangeOption();
    } else if (mpMeanOption && mpMeanOption->count()) {
        runMeanOption(); // can throw CLI::ValidationError
    } else if (mpSampleHistoryOption && mpSampleHistoryOption->count()) {
        runSampleHistoryOption();
    }
}

bool ParseWeatherDriver::checkDateRange(const std::string& range_string) const {

    if (range_string.size() != DateRangeLength) {
        return false;
    }

    const auto startUnix = jsonparse::dateToUnix(range_string.substr(0, 10));
    const auto finishUnix = jsonparse::dateToUnix(range_string.substr(11));

    return startUnix.has_value() 
        && finishUnix.has_value() 
        && startUnix <= finishUnix;
}

bool ParseWeatherDriver::readInputFile() {
    // Read the json file into a Json::Value object
    Json::Value schema;
    try {
        std::ifstream file(mInputFilename);
        schema = jsonparse::jsonFromString(
                {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()});

    } catch (const jsonparse::IncorrectJson& error) {
        std::cerr << "An error occured parsing the json file: " << error.what() << "\n";
        return false;
    }

    if (schema.isArray()) {
        // Check this is an array of weather data schema's
        for (auto i = 0; i < schema.size(); ++i) {
            const Json::Value weatherSchema = schema[i];
            try {
                mArchive.addData(jsonparse::parseWeather(weatherSchema));
            } catch (const jsonparse::IncorrectJson& error) {
                std::cerr << "An error occurred parsing the json file: "
                    << error.what() << "\n";
                return false;
            }
        }
    } else if (schema.isObject()) {
        // Check if file only contains a single weather data schema 
        try {
            mArchive.addData(jsonparse::parseWeather(schema));
        } catch (const jsonparse::IncorrectJson& error) {
            std::cerr << "An error occurred parsing the json file: "
                << error.what() << "\n";
            return false;
        }
    }

    return true;
}

void ParseWeatherDriver::runDateOption() const {
    // mOptionSingleString will contain the YYYY-MM-DD string to look up in mArchive
    const auto unixTime = jsonparse::dateToUnix(mOptionSingleString);
    if (unixTime.has_value()) {
        const auto dataOptional = mArchive.retrieve(unixTime.value());
        if (dataOptional.has_value()) {
            std::cout << 
                jsonparse::jsonPretty(jsonparse::createWeatherJson(dataOptional.value())) << "\n";
        } else {
            std::cerr << "Data for date: " << mOptionSingleString << " is not available\n";
        }
    } else {
        std::cerr << "An error occurred parsing the input date: " << mOptionSingleString << "\n";
    }
}

void ParseWeatherDriver::runRangeOption() const {
    const auto startUnix = jsonparse::dateToUnix(mOptionSingleString.substr(0, 10));
    const auto finishUnix = jsonparse::dateToUnix(mOptionSingleString.substr(11, 10));

    printWeatherData(mArchive.retrieveRange(startUnix.value(), finishUnix.value())); 
}

void ParseWeatherDriver::printWeatherData(const std::vector<WeatherData>& data) const {
    // create a json array for printing
    Json::Value outputArray = Json::arrayValue;
    for (const auto& data : data) {
        outputArray.append(jsonparse::createWeatherJson(data));
    }

    std::cout << jsonparse::jsonPretty(outputArray) << "\n";
}

// expecting there to be two inputs for this option!
void ParseWeatherDriver::runMeanOption() const {

    if (mOptionMultiString.size() != 2) { // cli11 should guarentee this
        throw CLI::ValidationError(
                "MeanOptionError",
                "Incorrect input for -m, --mean option. This option expects "
                "two inputs\n");
    }

    // one of the inputs should be a date range string, the other should be a
    // variable name
    if (checkDateRange(mOptionMultiString[0])) {
        const auto it = std::find(
                VariableStrings.cbegin(),
                VariableStrings.cend(),
                mOptionMultiString[1]);

        if (it != VariableStrings.end()) {
            const auto mean = calcVariableMean(mOptionMultiString[0], *it);
            if (std::isnan(mean)) {
                std::cerr << "Could not calculate a mean; data for variable \""
                    << *it << "\" is not present within the time range " 
                    << mOptionMultiString[0] << "\n";
            } else {
                std::cout << std::fixed << std::setprecision(3) << mean << "\n";
            }
        } else {
            throw CLI::ValidationError(
                    "MeanOptionError",
                    "Incorrect input for -m, --mean option. The variable \""
                    + mOptionMultiString[1] + "\" is not recognized\n");
        }
    } else if (checkDateRange(mOptionMultiString[1])) {
        const auto it = std::find(
                VariableStrings.cbegin(),
                VariableStrings.cend(),
                mOptionMultiString[0]);

        if (it != VariableStrings.end()) {
            const auto mean = calcVariableMean(mOptionMultiString[1], *it);
            if (std::isnan(mean)) {
                std::cerr << "Could not calculate a mean; data for variable \""
                    << *it << "\" is not present within the time range " 
                    << mOptionMultiString[1] << "\n";
            } else {
                std::cout << std::fixed << std::setprecision(3) << mean << "\n";
            }
        } else {
            throw CLI::ValidationError(
                    "MeanOptionError",
                    "Incorrect input for -m, --mean option. The variable \""
                    + mOptionMultiString[0] + "\" is not recognized\n");
        }
    } else {
        throw CLI::ValidationError(
                "MeanOptionError",
                "Incorrect input for -m, --mean option. This option expects one "
                "input to be a date range\n");
    }
}

double ParseWeatherDriver::calcVariableMean(
        const std::string& range_string, 
        const std::string& variable_name) const {
    const auto startUnix = jsonparse::dateToUnix(range_string.substr(0, 10));
    const auto finishUnix = jsonparse::dateToUnix(range_string.substr(11, 10));

    const auto rangeData = mArchive.retrieveRange(
            startUnix.value(), finishUnix.value()); 

    std::size_t count = 0;
    double sum = 0;
    if (variable_name == jsonparse::TMAX_KEY) {
        for (const auto& data : rangeData) {
            if (data.maxTemp.has_value()) {
                count++;
                sum += data.maxTemp.value();;
            } else {
                std::cerr << "Data for date: " << jsonparse::unixToDate(data.time.value())
                    << " is missing \"" << variable_name << "\" and will be ignored for "
                    "calcuating the mean\n";
            }
        }

    } else if (variable_name == jsonparse::TMIN_KEY) {
        for (const auto& data : rangeData) {
            if (data.minTemp.has_value()) {
                count++;
                sum += data.minTemp.value();;
            } else {
                std::cerr << "Data for date: " << jsonparse::unixToDate(data.time.value())
                    << " is missing \"" << variable_name << "\" and will be ignored for "
                    "calcuating the mean\n";
            }
        }
    } else if (variable_name == jsonparse::TMEAN_KEY) {
        for (const auto& data : rangeData) {
            if (data.meanTemp.has_value()) {
                count++;
                sum += data.meanTemp.value();;
            } else {
                std::cerr << "Data for date: " << jsonparse::unixToDate(data.time.value())
                    << " is missing \"" << variable_name << "\" and will be ignored for "
                    "calcuating the mean\n";
            }
        }
    } else if (variable_name == jsonparse::PPT_KEY) {
        for (const auto& data : rangeData) {
            if (data.gas_ppt.has_value()) {
                count++;
                sum += data.gas_ppt.value();;
            } else {
                std::cerr << "Data for date: " << jsonparse::unixToDate(data.time.value())
                    << " is missing \"" << variable_name << "\" and will be ignored for "
                    "calcuating the mean\n";
            }
        }
    }  // handling unrecognized variable name occurs within runMeanOption

    if (count > 0) {
        return sum / count;
    } else {
        return std::nan("");
    }
}

void ParseWeatherDriver::runSampleHistoryOption() const {
    if (mOptionMultiString.size() != 2) { // cli11 should guarentee this
        throw CLI::ValidationError(
                "SampleHistoryOptionError",
                "Incorrect input for -s, --sample-history option. This option expects "
                "two inputs\n");
    }

    // one of the inputs should be a date range string, the other should be a
    // year range string
    if (checkDateRange(mOptionMultiString[0]) 
            && checkYearRange(mOptionMultiString[1])) {
        printWeatherData(sampleHistoricalData(mOptionMultiString[0], mOptionMultiString[1]));
    } else if (checkDateRange(mOptionMultiString[1]) 
            && checkYearRange(mOptionMultiString[0])) {
        printWeatherData(sampleHistoricalData(mOptionMultiString[1], mOptionMultiString[0]));
    } else {
        throw CLI::ValidationError(
                "SampleHistoryOptionError",
                "Incorrect input for -s, --sample-history option.\n");
    }
}

bool ParseWeatherDriver::checkYearRange(const std::string& year_range) const {
    if (year_range.size() != YearRangeLength) {
        return false;
    }

    std::smatch match;
    if (!std::regex_match(year_range.cbegin(), year_range.cbegin() + 4, 
                match, jsonparse::yearRegex)
        || !std::regex_match(year_range.cbegin() + 5, year_range.cend(),
            match, jsonparse::yearRegex)) {
        return false;
    }

    // since regex validates years, stoi will not throw
    return std::stoi(year_range.substr(0, 4)) <= std::stoi(year_range.substr(5));
}

std::vector<WeatherData> ParseWeatherDriver::sampleHistoricalData(
            const std::string& date_range,
            const std::string& year_range) const {

    date::sys_days startDays;
    std::smatch searchResult;
    if (std::regex_match(
                date_range.cbegin(), date_range.cbegin() + 10,
                searchResult, jsonparse::dateRegex)) {
        // date library syntax is year/month/day
        startDays = date::year{std::stoi(searchResult.str(1))} /
            std::stoi(searchResult.str(2))/
            std::stoi(searchResult.str(3));
    } else { // should not occur since date_range has already been verified for correct format
        return {};
    }

    date::sys_days finishDays;
    if (std::regex_match(
                date_range.cbegin() + 11, date_range.cend(),
                searchResult, jsonparse::dateRegex)) {
        // date library syntax is year/month/day
        finishDays = date::year{std::stoi(searchResult.str(1))} /
            std::stoi(searchResult.str(2))/
            std::stoi(searchResult.str(3));
    } else {
        return {};
    }

    std::vector<WeatherData> retData; // vector that is returned

    const auto startSampleYears = std::stoi(year_range.substr(0, 4));
    const auto finishSampleYears = std::stoi(year_range.substr(5));

    // initialize a vector with the possible sample years
    std::vector<int> sampleYears(finishSampleYears - startSampleYears + 1);
    std::iota(sampleYears.begin(), sampleYears.end(), startSampleYears);

    auto numGenerator = std::mt19937{std::random_device{}()};  // random_device for shuffle
    for (auto i = startDays; i <= finishDays; ) {
        const date::year_month_day ymd = i;

        /* Randomly shuffle the vector containing the sample years.
         * Starting with the year in the front of the vector, attempt to get the data point
         * from that year. If there is no data point, try the next year.
         * If there is not data point at all, then this is an error!
        */
        std::shuffle(sampleYears.begin(), sampleYears.end(), numGenerator);

        auto dataOpt = [&]() -> std::optional<WeatherData> {
            for (const auto& year : sampleYears) {
                // create year_month_day object, then convert it UTM/GMT time,
                // and finally query mArchive for the data
                const auto sampleData = mArchive.retrieve(
                        std::chrono::duration_cast<std::chrono::seconds>(
                            date::sys_days{date::year{year}/ymd.month()/ymd.day()}.time_since_epoch()
                        ).count());

                if (sampleData.has_value()) {
                    return sampleData;
                }
            }

            return std::nullopt;
        }();

        if (dataOpt.has_value()) {
            // assign the time for the expected output
            dataOpt.value().time = std::chrono::duration_cast<std::chrono::seconds>(
                    i.time_since_epoch()).count();
            retData.push_back(dataOpt.value());
        } 

        i = i + date::days{1}; // move time by one day
    }

    return retData;
}
