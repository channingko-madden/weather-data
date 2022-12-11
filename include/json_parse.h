/**
 * @file json_parse.h
 * @date 12/9/2022
 *
 * @brief jsonparse namespace declaration
 */

#ifndef JSON_PARSE_H
#define JSON_PARSE_H

#include "data/weather_data.h"

#include <jsoncpp/json/value.h>
#include <string>
#include <regex>

/**
 * @namespace jsonparse
 * @brief Generic classes and functions for parsing JSON formatted data
 */
namespace jsonparse {

    /** @brief A regex for identifying yyyy-mm-dd date strings
     *  It captures the year, month, and day in capture groups
     */
    const std::regex dateRegex = std::regex("([12]\\d{3}-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01]))");

    /** @brief This struct defines the exception thrown to report an incorrect json argument */
    struct IncorrectJson : public std::exception
    {
      const std::string mError;

      /** @brief Constructor with default error message */
      IncorrectJson() : mError("JSON does not match payload format") {}

      /** @brief Constructor that sets error message 
       *  @param[in] error Error message */
      IncorrectJson(const std::string& error) : mError(error) {}

      const char* what() const throw()
      {
        return mError.c_str();
      }
    };

    /** 
     * @brief Return a JSON Schema created from a json string
     * @throws IncorrectJson if parsing the string fails
     * @param[in] json_string
     * @return JSON Schema
     */
    Json::Value json_from_string(const std::string& json_string) noexcept(false);

    /**
     * @brief Convert a YYYY-MM-DD date string to Unix (UTC) time
     * (Number of seconds since January 1st, 1970 UTC)
     * @param[in] date_string The date string in YYYY-MM-DD format
     * @return The corresponding UTC Unix time
     */
    std::optional<std::chrono::seconds::rep> dateToUnix(const std::string& date_string);

    /**
     * @brief Parse a JSON Schema containing weather data 
     * @param[in] schema A JSON object containing a JSON Schema for a weather data point. 
     *
     * A weather point Schmea contains the following key/value pairs:
     * - "date": string (format: "yyyy-mm-dd") [REQUIRED]
     * - "tmax": number
     * - "tmin": number
     * - "tmean": number
     * - "ppt": number
     *
     * If any key/value pair is missing, the returned object's optional will not be set
     * @throws IncorrectJson if the schema is not an object
     */
    WeatherData parseWeather(const Json::Value& schema) noexcept(false);

} // jsonparse
#endif // JSON_PARSE_H
