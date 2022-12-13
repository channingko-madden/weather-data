/**
 * @file weather_archive.h
 * @date 12/12/2022
 *
 * @brief WeatherArchive class declaration
 */

#ifndef WEATHER_ARCHIVE_H
#define WEATHER_ARCHIVE_H

#include "data/weather_data.h"

#include <map>
#include <optional>
#include <vector>

/**
 * @class WeatherArchive weather_archive.h "data/weather_archive.h"
 * @brief This class stores timestamped weather data, and provides 
 * methods for retrieving data based on timestamps
 */
class WeatherArchive {
public:

    /**
     * @brief Add a new weather data point into the archive
     *
     * If there already exists a data point in the archive with the same timestamp it
     * is replaced.
     * If the data does not have a timestamp it cannot be added to the archive, and 
     * this method does nothing.
     *
     * @param[in] data Weather data to add
     */
    void addData(const WeatherData& data);

    /**
     * @brief Retrieve a single data point that matches the input time
     * @param[in] time Timestamp of the data point
     * @return The corresponding WeatherData if the archive contains it, otherwise
     * the optional will not be set
     */
    std::optional<WeatherData> retrieve(const WeatherData::data_time time) const;

    /**
     * @brief Retrieve weather data from a specific UTM/GMT time range
     * @param[in] begin_sec The beginning of the time range (UTM/GMT time) in seconds
     * @param[in] end_sec The end of the time range, in seconds
     * @return All data within that time range. An empty vector denotes no data for the time range
     * is available.
     */
    std::vector<WeatherData> retrieveRange(
            const WeatherData::data_time begin_sec,
            const WeatherData::data_time end_sec) const;

private:

    /**@brief Store weather data in a map, using the time as the key.
     *
     * An ordered map is used so that ranges of data can be easily created
     */
    std::map<WeatherData::data_time, WeatherData> mWeatherMap;

};
#endif // WEATHER_ARCHIVE_H
