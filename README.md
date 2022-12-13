# weather-data
Weather data [technical assessment](docs/Technical_Task.pdf). 

## vcpkg (dependency-manager)
[vcpkg](https://github.com/microsoft/vcpkg) is used to manage the dependencies for this project.
Current dependencies are listed in [vcpkg.json](vcpkg.json). Currently they are:
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
* [date](https://github.com/HowardHinnant/date)
* [cli11](https://github.com/CLIUtils/CLI11)
* [googletest](https://github.com/google/googletest)

vcpkg has stable versions of the above libraries and makes it easy to download and build with them.
### One-time setup for vcpkg
vcpkg is included as a submodule within this project and requires a one-time setup when
this project repo is first cloned.
```bash
git submodule update --init
cd extern/vcpkg
./bootstrap-vcpkg.sh -disableMetrics
```

## Build, Test, & Run
### Build using cmake
You can build the parseweather command-line script as well as unit tests using cmake (C++17 is required).
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
### Install parseweather using cmake
The parseweather script and unit test executables can be run from within the build directory.\
Optionally, after building you can install the parseweather script also using cmake, and be able to
run the script from outside the build directory.
```bash
cd build
sudo cmake --install .
```
### Test Scripts
An example JSON data file is located within the test directory
- [json_parse_test](test/json_parse_test.cpp): Unit test for functions for parsing JSON data
- [weather_archive_test](test/weather_archive_test.cpp): Unit test for
[WeatherArchive](include/data/weather_archive.h) class

### parseweather
Instructions for how to use the parseweather command-line script can be found using the --help option:
```bash
parseweather -h
```
On successful execution, parseweather sends the output to stdout, which can be redirected into a file if desired.
For example:
```bash
parseweather -f example_weather.json -d 2016-01-01 > output.json
```
Warning and error messages are output to stderr to protect the JSON format of data output to stdout.

#### The | character
In the terminal, the | character will be interpreted as the pipe command, and therefore needs to be escaped when
using the --range, --mean, and --sample-history options.\
For example: 
```bash
parseweather -f example_weather.json -r 2022-01-01\|2022-12-31
```

#### Conflict of -h option 
The [technical assessment](docs/Technical_Task.pdf) asks that there be an option -h or --historical-sample as an
extra challenge. I decided to instead change the name of this option to -s or --sample-history because of the conflict
of the short option -h with the common convention of -h as the short option for --help. I felt it was worth keeping
in line with the -h, --help convention, and that changing the option name to -s, --sample-history did not make it
more difficult to understand.

## Doxygen Documentation
HTML API documentation can be created using doxygen.
```bash
cd docs
doxygen weather_data.conf
```
The html documentation is created and placed in a directory named html.\
Documentation can then be viewed in a browser. To open from the terminal:
```bash
cd docs
xdg-open html/index.html
```

## Future Improvements
* Improve user warning and error messages
  * When data is missing for range based options
  * When a historical sample cannot be found
* Functional tests for parseweather script
* Clang formatting
