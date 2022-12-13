/**
 * @file parse_weather.cpp
 * @date 12/12/2022
 *
 * @brief The parseweather cli script
 *
 * Run: parseweather -h for information on how to use the script
 */

#include "parse_weather_driver.h"
#include <CLI/CLI.hpp>

int main(int argc, char** argv) {

    CLI::App app {"A script that accepts a file with JSON formatted weather data and parses "
        "it according to the options below."};

    // Driver object that contains member fields for the various input fields
    ParseWeatherDriver driver;

    try {

        driver.setOptions(app);

        app.parse(argc, argv); // can throw CLI::Error

        driver.run(app); // can throw CLI::Error 

    } catch (const CLI::Error& error) {
        return app.exit(error);
    } 

    return 0;
}
