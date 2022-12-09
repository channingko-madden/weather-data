/**
 * @file payload_parser.h
 * @date 12/8/2022
 *
 * @brief payloadparser namespace declaration
 */

#ifndef PAYLOAD_PARSER_H
#define PAYLOAD_PARSER_H

#include "weather_data.h"

#include <jsoncpp/json/value.h>
#include <string>
#include <regex>

/**
 * @namespace payloadparser
 * @brief Classes and functions for parsing the JSON formatted weather data
 */
namespace payloadparser {

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

    /** @brief Return a JSON Schema created from a json string
     *  @throws IncorrectJson if parsing the string fails
     *  @param[in] json_string
     *  @return JSON Schema */
    Json::Value json_from_string(const std::string& json_string) noexcept(false);

    /**
     * @brief Parse a JSON Schema containing weather data 
     * @param[in] json_string A JSON formatted string containing a JSON Schema for a weather data point.
     * A weather point Schmea contains the following key/value pairs:
     * - "date": string (format: "yyyy-mm-dd")
     * - "tmax": number
     * - "tmin": number
     * - "tmean": number
     * - "ppt": number
     *
     * If any number type key/value pair is missing, the returned WeatherData object will contain a NaN value for 
     * that variable.
     * @throws IncorrectJson if parsing the string fails, or if the Schema is missing the "date" key
     * @return Weather data object with values initialized by the JSON Schema
     */
    WeatherData parseWeather(const Json::Value& schema) noexcept(false);

}
#endif
