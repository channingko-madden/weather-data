/**
 * @file parse_weather_driver.h
 * @date 12/9/2022
 *
 * @brief The driver class for the parseweather cli script
 */

#ifndef PARSE_WEATHER_DRIVER_H
#define PARSE_WEATHER_DRIVER_H

#include <CLI/CLI.hpp>
#include <string>

/**
 * @class ParseWeatherDriver parse_weather_driver.cpp "parse_weather_driver.h"
 * @brief This class contains the driver code and logic for the parseweather
 * command-line script
 */
class ParseWeatherDriver {
public:

    static bool validateApp(const CLI::App& app);

    /**
     * @brief Constructor
     * Pass the parsed command-line arguments to the driver
     *
     * Loads the input json file data
     */
    ParseWeatherDriver(const std::string& json_file);

    /**
     * @brief Run the script based on the command-line arguments
     */
    void run(CLI::App& app);

private:

    void readFile();

};
#endif // PARSE_WEATHER_DRIVER_H
