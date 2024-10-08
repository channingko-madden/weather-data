/**
 * @file weather_data.cpp
 * @date 12/12/2022
 *
 * @brief WeatherData class definition
 */

#include "data/weather_data.h"

std::ostream& operator << (std::ostream& out, const WeatherData& data) {
    out << "time:\t";
    if (data.time.has_value()) {
        out << data.time.value();
    } 

    out << "\nmaxTemp:\t";
    if (data.maxTemp.has_value()) {
        out << data.maxTemp.value();
    }

    out << "\nminTemp:\t";
    if (data.minTemp.has_value()) {
        out << data.minTemp.value();
    }

    out << "\nmeanTemp:\t";
    if (data.meanTemp.has_value()) {
        out << data.meanTemp.value();
    }

    out << "\ngas_ppt:\t";
    if (data.gas_ppt.has_value()) {
        out << data.gas_ppt.value();
    }

    return out;
}

bool WeatherData::operator!= (const WeatherData& other) const {
    return time != other.time || maxTemp != other.maxTemp 
        || minTemp != other.minTemp || meanTemp != other.meanTemp
        || gas_ppt != other.gas_ppt;
}

bool WeatherData::operator== (const WeatherData& other) const {
    return time == other.time && maxTemp == other.maxTemp 
        && minTemp == other.minTemp && meanTemp == other.meanTemp
        && gas_ppt == other.gas_ppt;
}
