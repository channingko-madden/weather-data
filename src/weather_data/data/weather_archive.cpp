/**
 * @file weather_archive.cpp
 * @date 12/9/2022
 *
 * @brief WeatherArchive class definition
 */

#include "data/weather_archive.h"

void WeatherArchive::addData(const WeatherData& data) {
    if (data.time.has_value()) {
        mWeatherMap[data.time.value()] = data;
    }
}

std::optional<WeatherData> WeatherArchive::retrieve(
        const WeatherData::data_time time) const {
    const auto it = mWeatherMap.find(time);
    if (it != mWeatherMap.end()) {
        return std::optional<WeatherData>(it->second);
    } else {
        return std::optional<WeatherData>();
    }
}

std::vector<WeatherData> WeatherArchive::retrieveRange(
        const WeatherData::data_time begin_sec,
        const WeatherData::data_time end_sec) const {

    // the beginning of the range must be before the end of the range
    if (begin_sec <= end_sec) {
        const auto begin_it = mWeatherMap.find(begin_sec);
        // at least the beginning of the range needs to be present
        if (begin_it != mWeatherMap.end()) {
            const auto end_it = mWeatherMap.find(end_sec);
            // regardless if end_sec is within the map or not
            // (aka end_it = map.end()) this is valid
            std::vector<WeatherData> retData;
            retData.reserve(std::distance(begin_it, end_it));
            for (auto it = begin_it; it != mWeatherMap.end(); ++it) {
                retData.push_back(it->second);
                if (it == end_it) {
                    break;
                }
            }

            return retData;
        }
    }

    return std::vector<WeatherData>();
}
