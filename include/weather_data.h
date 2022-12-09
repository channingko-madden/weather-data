/**
 * @file weather_data.h
 * @date 12/8/2022
 *
 * @brief WeatherData class declaration
 */

#ifndef WEATHER_DATA_H
#define WEATHER_DATA_H

#include <chrono>

using data_time = std::chrono::seconds::rep;

/**
 * @class WeatherData weather_data.h "weather_data.h"
 * @brief This class represents a weather data reading
 */
class WeatherData {
public:

    data_time time; /**<@brief Data's timestamp, in GMT/UTC time */
    float maxTemp; /**<@brief Maxiumum daily temperature in Celcius (C) */
    float minTemp; /**<@brief Minimum daily temperature (C) */
    float meanTemp; /**<@brief Mean daily temperature (C) */
    float gas_ppt; /**<@brief Concentration of a gas in the atmosphere, in parts per trillion (ppt) */

};
#endif
