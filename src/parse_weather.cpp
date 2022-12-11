/**
 * @file parse_weather.cpp
 * @date 12/8/2022
 *
 * @brief The parseweather cli script
 */

#include "parse_weather_driver.h"
#include "date/date.h"
#include <CLI/CLI.hpp>

int main(int argc, char** argv) {

    CLI::App app {"A script that takes in json formatted weather data and parses it according"
        " to the command you give it"};

    // Driver object that contains member fields for the various input fields
    ParseWeatherDriver driver;

    try {

        driver.setOptions(app);

        app.parse(argc, argv);

        driver.run(app);

    } catch (const CLI::Error& error) {
        return app.exit(error);
    } 

    return 0;
}
