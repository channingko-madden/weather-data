/**
 * @file parse_weather.cpp
 * @date 12/8/2022
 *
 * @brief The parseweather cli script
 */

#include "date/date.h"
#include "payload_parser.h"
#include "weather_data.h"
#include <iostream>

int main(int argc, char** argv) {

    WeatherData data;
    auto testDay = date::year{2017}/date::apr/12;
    const auto testUnix = date::sys_days{testDay};

    std::chrono::seconds::rep secondsTime = std::chrono::duration_cast<std::chrono::seconds>(testUnix.time_since_epoch()).count();

    std::cout << "Apr 12 2017 test: " << " unix " << 
        std::chrono::duration_cast<std::chrono::seconds>(testUnix.time_since_epoch()).count() << "\n" <<
        secondsTime << std::endl;

    return 0;
}
