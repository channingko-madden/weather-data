/**
 * @file json_parse.cpp
 * @date 12/8/2022
 *
 * @brief jsonparse namespace definition
 */

#include "json_parse.h"

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include "date/date.h"
#include <cmath>
#include <regex>
#include <memory>
#include <chrono>
#include <sstream>

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

    std::string jsonPretty(const Json::Value& schema) {
        Json::StreamWriterBuilder wbuilder;
        wbuilder["precision"] = 6; // limit total precision to 6 
        return Json::writeString(wbuilder, schema);
    }

    std::optional<std::chrono::seconds::rep> dateToUnix(const std::string& date_string) {
        // check that string contains a yyyy-mm-dd and capture year, month, day
        // using regex
        std::smatch searchResult;
        if (std::regex_search(date_string, searchResult, dateRegex)) {
            // date library syntax is year/month/day
            const auto date = date::year{std::stoi(searchResult.str(1))} /
                std::stoi(searchResult.str(2))/
                std::stoi(searchResult.str(3));
            // convert to UTM/GMT time
            return std::chrono::duration_cast<std::chrono::seconds>(
                    date::sys_days{date}.time_since_epoch()).count();
        }  else {
            return std::nullopt;
        }
    }

    std::string unixToDate(const std::chrono::seconds::rep& unix_time_sec) {

        const auto dayObj = date::year_month_day{date::floor<date::days>(date::sys_seconds{
                std::chrono::seconds(unix_time_sec)})};

        std::ostringstream daybuffer;
        daybuffer << dayObj;
        return daybuffer.str();
    }

    WeatherData parseWeather(const Json::Value& schema) {
        if (!schema.isObject()) {
            throw IncorrectJson();
        }

        WeatherData data;

        if (schema.isMember("date") && schema["date"].isString()) {
            data.time = dateToUnix(schema["date"].asString());
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

    Json::Value createWeatherJson(const WeatherData& weather_data) {
        Json::Value root;
        if (weather_data.time.has_value()) {
            root["date"] = unixToDate(weather_data.time.value());
        }

        if (weather_data.maxTemp.has_value()) {
            root["tmax"] = weather_data.maxTemp.value();
        }

        if (weather_data.minTemp.has_value()) {
            root["tmin"] = weather_data.minTemp.value();
        }

        if (weather_data.meanTemp.has_value()) {
            root["tmean"] = weather_data.meanTemp.value();
        }

        if (weather_data.gas_ppt.has_value()) {
            root["ppt"] = weather_data.gas_ppt.value();
        }

        return root;
    }

} // jsonparse

