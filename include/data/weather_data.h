/**
 * @file weather_data.h
 * @date 12/12/2022
 *
 * @brief WeatherData class declaration
 */

#ifndef WEATHER_DATA_H
#define WEATHER_DATA_H

#include <chrono>
#include <optional>
#include <ostream>
#include <string>


/**
 * @class WeatherData weather_data.h "weather_data.h"
 * @brief This class represents a weather data reading, where measurements may or may not be present
 *
 */
class WeatherData {
public:

    /** Declare the type for a timestamp */
    using data_time = std::chrono::seconds::rep;

    /**<@brief The data's timestamp, in seconds, GMT/UTC time, if available */
    std::optional<data_time> time; 
    
    /**<@brief The maximum daily temperature in Celcius, if available */
    std::optional<float> maxTemp;
    
    /**<@brief The minimum daily temperature in Celcius, if available */
    std::optional<float> minTemp;
 
    /**<@brief The mean daily temperature in Celcius, if available */
    std::optional<float> meanTemp;
 
    /**<@brief The concentration of a gas in the atmosphere, in parts per trillion (ppt), if available */
    std::optional<float> gas_ppt;

    // Overload comparison operators b/c they are not default created (compiler warning)
    bool operator!= (const WeatherData& other) const;
    bool operator== (const WeatherData& other) const;

    // Overload ostream operator for easily displaying data contents for debugging
    friend std::ostream& operator<< (std::ostream& out, const WeatherData& data);

};
#endif // WEATHER_DATA_H
