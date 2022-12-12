/**
 * @file parse_weather_driver.h
 * @date 12/12/2022
 *
 * @brief The driver class for the parseweather cli script
 */

#ifndef PARSE_WEATHER_DRIVER_H
#define PARSE_WEATHER_DRIVER_H

#include "json_parse.h"
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
     * @brief Strings denoting weather data variable names that are accepted
     * by the --mean option
     */
    const std::vector<std::string> VariableStrings{
        jsonparse::TMAX_KEY, jsonparse::TMIN_KEY,
        jsonparse::TMEAN_KEY, jsonparse::PPT_KEY};

    /**
     * @brief Set the parseweather script options on the app object
     * @param[in] app App object used for parsing CLI inputs
     */
    void setOptions(CLI::App& app);

    /**
     * @brief Run the script based on the command-line arguments
     * @throws CLI::ValidationError if invalid inputs are passed to 
     * any options.
     */
    void run(CLI::App& app) noexcept(false);

private:

    /**
     * @brief Check the validity of a date range in the format YYYY-MM-DD|YYYY-MM-DD
     * A valid date range string has the following requirements
     * - It is DateRangeLength in size
     * - The substrings before and after the '|' character conform the to proper date format
     * - The first date is a time before the second date
     *
     * @param[in] range_string
     * @return True if the string exactly matches the format, false otherwise
     */
    bool checkRangeString(const std::string& range_string) const;

    /**
     * @brief Read a json data file containing weather data, and store the
     * data within the WeatherArchive member variable mArchive
     *
     * If the data file cannot be read, mArchive will not be modified
     */
    bool readInputFile();

    /**
     * @brief Run functionality for the --date option
     * Validity of the input has already be checked by the parser
     */
    void runDateOption() const;

    /**
     * @brief Run functionality of the --range option
     * Validity of the input has already be checked by the parser
     */
    void runRangeOption() const;

    /**
     * @brief Run the functionality of the --mean option
     *
     * Checks the validity of the inputs passed to the mean option (-m, --mean)
     * Allowed inputs are (in any order):
     * - A date range: YYYY-MM-DD|YYYY-MM-DD
     * - A string denoting the variable: tmax, tmin, tmean, or ppt
     * @throws CLI::ValidationError if inputs are not valid
     */
    void runMeanOption() const noexcept(false);

    /**
     * @brief Calculate the mean for a given variable, over a given date range
     *
     * If a variable is missing for a given day within the range, it is ignored
     * as part of the calculation
     * @param[in] range_string A date range string: YYYY-MM-DD|YYYY-MM-DD
     * @param[in] variable_name A string denoting the variable (ex. "tmax")
     * @return The calculated mean, or NaN if either variable_name is unrecognized
     * of the variable is missing from the entire date range
     */
    double calcVariableMean(
            const std::string& range_string,
            const std::string& variable_name) const;

    /**
     * @brief Run the functionality for the --sample option
     *
     * Checks the validity of the inputs passed to the sample option
     * Allowed inputs are (in exact order):
     * - A date range: YYYY-MM-DD|YYYY-MM-DD
     * - A year range: YYYY|YYYY
     * @throws CLI::ValidationError if inputs are not valid
     */
    void runSampleHistoryOption() const noexcept(false);

    // Option pointers
    CLI::Option* mpFileOption {nullptr};
    CLI::Option* mpDateOption {nullptr};
    CLI::Option* mpRangeOption {nullptr};
    CLI::Option* mpMeanOption {nullptr};
    CLI::Option* mpSampleHistoryOption {nullptr};

    std::string mInputFilename; /**<@brief Absolute file path for input json file */
    std::string mOutputFilename; /**<@brief Absolute file path for output json file */
    std::string mOptionSingleString; /**<@brief String passed to an option that accepts a single string */
    /**@brief Strings passed to an option that accepts multiple values */
    std::vector<std::string> mOptionMultiString;

    WeatherArchive mArchive;

};
#endif // PARSE_WEATHER_DRIVER_H
