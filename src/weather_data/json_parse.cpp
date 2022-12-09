/**
 * @file json_parse.cpp
 * @date 12/8/2022
 *
 * @brief jsonparse namespace definition
 */

#include "json_parse.h"

#include <jsoncpp/json/reader.h>
#include "date/date.h"
#include <cmath>
#include <regex>
#include <memory>
#include <chrono>

namespace jsonparse {

    Json::Value json_from_string(const std::string& json_string) {
        Json::CharReaderBuilder builder;
        const auto reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());
        Json::Value json;
        std::string errors;
        if(reader->parse(json_string.c_str(), json_string.c_str() + json_string.size(), &json, &errors))
            return json;
        else
            throw IncorrectJson(errors);
    }

    WeatherData parseWeather(const Json::Value& schema) {
        if (!schema.isObject()) {
            throw IncorrectJson();
        }

        WeatherData data;

        if (schema.isMember("date") && schema["date"].isString()) {
            const auto dateString = schema["date"].asString();
            // check that string contains a yyyy-mm-dd and capture year, month, day
            // using regex
            std::smatch searchResult;
            if (std::regex_search(dateString, searchResult, dateRegex)) {
                // date library syntax is year/month/day
                const auto date = date::year{std::stoi(searchResult.str(1))} /
                    std::stoi(searchResult.str(2))/
                    std::stoi(searchResult.str(3));
                // convert to UTM/GMT time
                data.time = std::chrono::duration_cast<std::chrono::seconds>(
                        date::sys_days{date}.time_since_epoch()).count();
            } 
        }

        if (schema.isMember("tmax") && schema["tmax"].isNumeric()) {
            data.maxTemp = schema["tmax"].asDouble();
        }

        if (schema.isMember("tmin") && schema["tmin"].isNumeric()) {
            data.minTemp = schema["tmin"].asDouble();
        } 

        if (schema.isMember("tmean") && schema["tmean"].isNumeric())  {
            data.meanTemp = schema["tmean"].asDouble();
        } 

        if (schema.isMember("ppt") && schema["ppt"].isNumeric())  {
            data.gas_ppt = schema["ppt"].asDouble();
        } 

        return data;
    }

} // jsonparse

