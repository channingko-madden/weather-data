/**
 * @file parse_weather_driver.h
 * @date 12/9/2022
 *
 * @brief The driver class for the parseweather cli script
 */

#ifndef PARSE_WEATHER_DRIVER_H
#define PARSE_WEATHER_DRIVER_H

#include "data/weather_archive.h"

#include <CLI/CLI.hpp>
#include <string>
#include <vector>

/**
 * @class ParseWeatherDriver parse_weather_driver.cpp "parse_weather_driver.h"
 * @brief This class contains the driver code and logic for the parseweather
 * command-line script
 */
class ParseWeatherDriver {
public:

    /** 
     * @brief The length of a string containing a date range in the accept format
     * of YYYY-MM-DD|YYYY-MM-DD
     */
    static constexpr int DateRangeLength = 21;

    /**
     * @brief Set the parseweather script options on the app object
     * @param[in] app App object used for parsing CLI inputs
     */
    void setOptions(CLI::App& app);

    /**
     * @brief Run the script based on the command-line arguments
     */
    void run(CLI::App& app);

private:

    /**
     * @brief Check the validity of a date range in the format YYYY-MM-DD|YYYY-MM-DD
     * A valid date range string has the following requirements
     * - It is DateRangeLength in size
     * - The substrings before and after the '|' character conform the to proper date format
     * - The first date is a timem before the second date
     *
     * @param[in] range_string
     * @return True if the string exactly matches the format, false otherwise
     */
    bool checkRangeString(const std::string& range_string) const;

    /**
     * @brief Check the validity of the inputs passed to the mean option (-m, --mean)
     * Allowed inputs are:
     * - A date range: YYYY-MM-DD|YYYY-MM-DD
     * - A string denoting the variable: tmax, tmin, tmean, or ppt
     * @param[in] mean_inputs The parsed inputs for the mean option
     * @return True if inputs are valid, otherwise false
     */
    bool checkMeanOption(const std::vector<std::string>& mean_inputs) const;

    /**
     * @brief Read a json data file containing weather data, and store the
     * data within the WeatherArchive member variable mArchive
     * @param[in] filename Absolute path to json data file
     *
     * If the data file cannot be read, mArchive will not be modified
     */
    void readDataFile(const std::string& filename);

    // Option pointers
    CLI::Option* mpFileOption {nullptr};
    CLI::Option* mpDateOption {nullptr};
    CLI::Option* mpRangeOption {nullptr};
    CLI::Option* mpMeanOption {nullptr};
    CLI::Option* mpHistoricalOption {nullptr};

    std::string mInputFilename; /**<@brief Absolute file path for input json file */
    std::string mOutputFilename; /**<@brief Absolute file path for output json file */
    std::string mOptionSingleString; /**<@brief String passed to an option that accepts a single string */
    /**@brief Strings passed to an option that accepts multiple values */
    std::vector<std::string> mOptionMultiString;

    WeatherArchive mArchive;

};
#endif // PARSE_WEATHER_DRIVER_H
