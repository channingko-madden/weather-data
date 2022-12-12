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
     * @brief The length of a string containing a year range in the accept format
     * of YYYY|YYYY
     */
    static constexpr int YearRangeLength = 9;

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
     * - The first date is a time at or before the second date
     *
     * @param[in] range_string
     * @return True if the string exactly matches the format, false otherwise
     */
    bool checkDateRange(const std::string& range_string) const;

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
     * @brief Print weather data within a JSON Array
     * @param[in] data Weather data to print
     */
    void printWeatherData(const std::vector<WeatherData>& data) const;

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

    /**
     * @brief Check the validity of a year range in the format YYYY|YYYY
     * A valid year range string has the following requirements
     * - It is YearRangeLength in size
     * - The substrings before and after the '|' character conform the to proper year format
     * - The first year is chronologically at or before the second year
     *
     * @param[in] year_range A year range string in the format YYYY|YYYY
     * @return True if the string exactly matches the format, false otherwise
     */
    bool checkYearRange(const std::string& year_range) const;

    /**
     * @brief Create a collection of weather data with dates for each day specified by the
     * date_range parameter. However, the data returned for each date is determined by
     * first randomly choosing a year that is within the range specified by the year_range
     * parameter, then returning the data from the same day & month of the randomly 
     * selected year. If there is no avaialable data within the year_range for a 
     * given date, it is ommitted from the returned data.
     *
     * Assumes the passed parameters are in the correct format, the validity of the
     * parameters is not checked
     *
     * @param[in] date_range A date range string: YYYY-MM-DD|YYYY-MM-DD
     * @param[in] year_range A year range string: YYYY|YYYY
     *
     * @return The historically sampled data. An empty vector denotes that there is no data 
     * available within the date_range for any year requested.
     */
    std::vector<WeatherData> sampleHistoricalData(
            const std::string& date_range,
            const std::string& year_range) const;

    // Option pointers
    CLI::Option* mpFileOption {nullptr};
    CLI::Option* mpDateOption {nullptr};
    CLI::Option* mpRangeOption {nullptr};
    CLI::Option* mpMeanOption {nullptr};
    CLI::Option* mpSampleHistoryOption {nullptr};

    std::string mInputFilename; /**<@brief Absolute file path for input json file */
    std::string mOptionSingleString; /**<@brief String passed to an option that accepts a single string */
    /**@brief Strings passed to an option that accepts multiple values */
    std::vector<std::string> mOptionMultiString;

    WeatherArchive mArchive;

};
#endif // PARSE_WEATHER_DRIVER_H
