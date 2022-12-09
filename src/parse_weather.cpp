/**
 * @file parse_weather.cpp
 * @date 12/8/2022
 *
 * @brief The parseweather cli script
 */

#include "parse_weather_driver.h"
#include "date/date.h"
#include <CLI/CLI.hpp>
#include <string>

int main(int argc, char** argv) {

    auto testDay = date::year{2017}/date::apr/12;
    const auto testUnix = date::sys_days{testDay};

    std::chrono::seconds::rep secondsTime = std::chrono::duration_cast<std::chrono::seconds>(testUnix.time_since_epoch()).count();

    std::cout << "Apr 12 2017 test: " << " unix " << 
        std::chrono::duration_cast<std::chrono::seconds>(testUnix.time_since_epoch()).count() << "\n" <<
        secondsTime << std::endl;

    CLI::App app {"A script that takes in json formatted weather data and parses it according"
        " to the command you give it"};

    try {
        // json input file 
        std::string filename {"example_weather.json"};
        app.add_option("-f, --file", filename, "Absolute path to json weather data file.\n"
                "Ex: parseweather -f /home/path/to/file.json");

        // date option
        std::string dateQuery;
        app.add_option("-d, --date", dateQuery, "A specific day to retrieve weather data for, "
                "formatted as YYYY-MM-DD\n"
                "Ex: -d 2022-01-01");

        std::string rangeQuery;
        app.add_option("-r, --range", rangeQuery, "Return all weather data from the specific time range. "
                "Formatted as YYYY-MM-DD|YYYY-MM-DD\n"
                "Ex: -r 2022-01-01|2022-12-31");

        std::vector<std::string> meanQuery;
        app.add_option("-m, --mean", meanQuery, "Return the mean of the variable provided over the "
                "specific time range. Time range formatted as YYYY-MM-DD|YYYY-MM-DD. Possible variable "
                " options are: tmax, tmin, tmean, and ppt.\n"
                "Ex: -m 2022-01-01|2022-12-31 tmax ");
        

    } catch (const CLI::ParseError& perror) {
        return app.exit(perror);
    } 



    return 0;
}
