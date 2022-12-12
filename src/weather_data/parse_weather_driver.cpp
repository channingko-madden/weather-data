/**
 * @file parse_weather_driver.cpp
 * @date 12/12/2022
 *
 * @brief The driver class for the parseweather cli script
 */

#include "parse_weather_driver.h"
#include "json_parse.h"

#include "jsoncpp/json/value.h"
#include <algorithm>
#include <regex>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>

void ParseWeatherDriver::setOptions(CLI::App& app) {
    // json input file is required, is checked that the file exists
    mpFileOption = app.add_option(
            "-f, --file",
            mInputFilename,
            "Absolute path to json weather data file.\n"
            "Ex: parseweather -f /home/path/to/file.json")
        ->required()
        ->check(CLI::ExistingFile);
                

    /*
    std::string outFilename {"weather_output.json"};
    app.add_option(
            "-o, --out",
            outFilename,
    */

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
                    throw CLI::ValidationError("DateOptionError", "Incorrect input for -d, --date option");
                }
            });

    // range option
    mpRangeOption = app.add_option(
            "-r, --range",
            mOptionSingleString,
            "Return all weather data from the specific time range. "
            "Formatted as YYYY-MM-DD|YYYY-MM-DD\n"
            "Ex: -r 2022-01-01|2022-12-31\n"
            "\t(note: In the terminal, the | character will be interpreted as the "
            "pipe command. Therefore it needs to be escaped.\n"
            "\tex: 2022-01-01\\|2022-12-31)\n")
        ->excludes(mpDateOption) // exludes so that only one option is accepted at a time
        ->check([this](const std::string& str) {
            if (checkRangeString(str)) {
                return std::string();
            } else {
                throw CLI::ValidationError("RangeOptionError", "Incorrect input for -r, --range option");
            }
        });

    // mean option, validity is easier checked with the parsed contents
    mpMeanOption = app.add_option(
            "-m, --mean",
            mOptionMultiString,
            "Return the mean of the variable provided over the "
            "specific time range. Time range formatted as YYYY-MM-DD|YYYY-MM-DD."
            "\nPossible variable options are: tmax, tmin, tmean, and ppt."
            "\nEx: -m 2022-01-01|2022-12-31 tmax  or -m tmax 2022-01-01|2022-12-31")
        ->expected(2)
        ->excludes(mpDateOption) // excludes so that only one option is accepted at a time
        ->excludes(mpRangeOption);

    // historical sample option, validity is easier checked with the parsed contents
    mpSampleHistoryOption = app.add_option(
            "-s, --sample-historical",
            mOptionMultiString,
            "Return weather data similar to the --range option, except that for each date "
            "within the range, randomly select a year from the second option,\n"
            "and return the data from the same day of the randomly chosen year."
            "\nInput is expected in the format: YYYY-MM-DD|YYYY-MM-DD YYYY|YYYY"
            "\nEx: -s 2022-01-01|2022-12-31 2018|2022")
        ->expected(2)
        ->excludes(mpDateOption) // excludes so that only one option is accepted at a time
        ->excludes(mpRangeOption)
        ->excludes(mpMeanOption);
}

void ParseWeatherDriver::run(CLI::App& app) {
    std::cout << "Run called\n";

    if (mpFileOption && mpFileOption->count()) { // This should always be true since this is required, but be safe and check
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
    std::cout << "Run finished\n";
}

bool ParseWeatherDriver::checkRangeString(const std::string& range_string) const {

    if (range_string.size() != DateRangeLength) {
        //std::cout << "Range string is not proper length\n";
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
        schema = jsonparse::json_from_string(
                {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()});

    } catch (const jsonparse::IncorrectJson& error) {
        std::cout << "An error occured parsing the json file: " << error.what() << "\n";
        return false;
    }

    if (schema.isArray()) {
        // Check this is an array of weather data schema's
        for (auto i = 0; i < schema.size(); ++i) {
            const Json::Value weatherSchema = schema[i];
            try {
                mArchive.addData(jsonparse::parseWeather(weatherSchema));
            } catch (const jsonparse::IncorrectJson& error) {
                std::cout << "An error occurred parsing the json file: "
                    << error.what() << "\n";
                return false;
            }
        }
    } else if (schema.isObject()) {
        // Check if file only contains a single weather data schema 
        try {
            mArchive.addData(jsonparse::parseWeather(schema));
        } catch (const jsonparse::IncorrectJson& error) {
            std::cout << "An error occurred parsing the json file: "
                << error.what() << "\n";
            return false;
        }
    }

    std::cout << "Loaded the json file\n";
    return true;
}

void ParseWeatherDriver::runDateOption() const {
    // mOptionSingleString will contain the YYYY-MM-DD string to look up in mArchive
    const auto unixTime = jsonparse::dateToUnix(mOptionSingleString);
    if (unixTime.has_value()) {
        std::cout << "Unix time: " << unixTime.value() << "\n";
        const auto dataOptional = mArchive.retrieve(unixTime.value());
        if (dataOptional.has_value()) {
            //std::cout << dataOptional.value() << "\n";
            std::cout << 
                jsonparse::jsonPretty(jsonparse::createWeatherJson(dataOptional.value())) << "\n";
        } else {
            std::cout << "The date: " << mOptionSingleString << " is missing from the data\n";
        }
    } else {
        std::cout << "An error occurred parsing the input date: " << mOptionSingleString << "\n";
    }
}

void ParseWeatherDriver::runRangeOption() const {
    const auto startUnix = jsonparse::dateToUnix(mOptionSingleString.substr(0, 10));
    const auto finishUnix = jsonparse::dateToUnix(mOptionSingleString.substr(11, 10));

    const auto rangeData = mArchive.retrieveRange(startUnix.value(), finishUnix.value()); 

    // create a json array for printing
    Json::Value outputArray = Json::arrayValue;
    for (const auto& data : rangeData) {
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
    
    if (checkRangeString(mOptionMultiString[0])) {
        const auto it = std::find(
                VariableStrings.cbegin(),
                VariableStrings.cend(),
                mOptionMultiString[1]);

        if (it != VariableStrings.end()) {
            const auto mean = calcVariableMean(mOptionMultiString[0], *it);
            std::cout << "Time range: " << mOptionMultiString[0] << "\t" <<
                " Variable: " << *it << "\n";
            if (std::isnan(mean)) {
                std::cout << "Could not calculate a mean; data for variable \""
                    << *it << "\" is not present within the time range " 
                    << mOptionMultiString[0] << "\n";
            } else {
                std::cout << "mean: " << std::fixed << std::setprecision(3) 
                    << mean << "\n";
            }
        } else {
            throw CLI::ValidationError(
                    "MeanOptionError",
                    "Incorrect input for -m, --mean option. The variable \""
                    + mOptionMultiString[1] + "\" is not recognized\n");
        }

    } else if (checkRangeString(mOptionMultiString[1])) {
        const auto it = std::find(
                VariableStrings.cbegin(),
                VariableStrings.cend(),
                mOptionMultiString[0]);

        if (it != VariableStrings.end()) {
            const auto mean = calcVariableMean(mOptionMultiString[1], *it);
            std::cout << "Time range: " << mOptionMultiString[1] << "\t" <<
                " Variable: " << *it << "\n" ;
            if (std::isnan(mean)) {
                std::cout << "Could not calculate a mean; data for variable \""
                    << *it << "\" is not present within the time range " 
                    << mOptionMultiString[1] << "\n";
            } else {
                std::cout << "mean: " << std::fixed << std::setprecision(3) 
                    << mean << "\n";
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

double ParseWeatherDriver::calcVariableMean(const std::string& range_string, 
        const std::string& variable_name) const {
    const auto startUnix = jsonparse::dateToUnix(range_string.substr(0, 10));
    const auto finishUnix = jsonparse::dateToUnix(range_string.substr(11, 10));

    const auto rangeData = mArchive.retrieveRange(startUnix.value(), finishUnix.value()); 

    std::size_t count = 0;
    double sum = 0;
    if (variable_name == jsonparse::TMAX_KEY) {
        for (const auto& data : rangeData) {
            if (data.maxTemp.has_value()) {
                count++;
                sum += data.maxTemp.value();;
            }
        }

    } else if (variable_name == jsonparse::TMIN_KEY) {
        for (const auto& data : rangeData) {
            if (data.minTemp.has_value()) {
                count++;
                sum += data.minTemp.value();;
            }
        }
    } else if (variable_name == jsonparse::TMEAN_KEY) {
        for (const auto& data : rangeData) {
            if (data.meanTemp.has_value()) {
                count++;
                sum += data.meanTemp.value();;
            }
        }
    } else if (variable_name == jsonparse::PPT_KEY) {
        for (const auto& data : rangeData) {
            if (data.gas_ppt.has_value()) {
                count++;
                sum += data.gas_ppt.value();;
            }
        }
    }

    if (count > 0) {
        return sum / count;
    } else {
        return std::nan("");
    }
}

void ParseWeatherDriver::runSampleHistoryOption() const {

}

