# @file CMakeLists.txt
# @date 12/8/2022

cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/extern/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")

project(weather-data VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(WD_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(jsoncpp REQUIRED)
find_package(CLI11 REQUIRED)
find_package(GTest REQUIRED)
find_package(date REQUIRED)

set(LIB_SOURCES
    ${WD_SOURCE_DIR}/weather_data/json_parse.cpp
    ${WD_SOURCE_DIR}/weather_data/data/weather_data.cpp
    ${WD_SOURCE_DIR}/weather_data/data/weather_archive.cpp
)

add_library(WeatherData SHARED ${LIB_SOURCES})
target_include_directories(WeatherData PUBLIC
    ${WD_INCLUDE_DIR}
)
target_compile_features(WeatherData PRIVATE
    cxx_std_17
)
target_link_libraries(WeatherData
    jsoncpp
    date::date
)

## parseweather script ##
add_executable(parseweather
    ${WD_SOURCE_DIR}/parse_weather.cpp
    ${WD_SOURCE_DIR}/weather_data/parse_weather_driver.cpp
)
target_include_directories(parseweather PUBLIC
    ${WD_INCLUDE_DIR}
)
target_compile_features(parseweather PRIVATE
    cxx_std_17
)
target_link_libraries(parseweather PRIVATE
    WeatherData
    CLI11::CLI11
)

#install(TARGETS appmap-app
#    BUNDLE DESTINATION .
##    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

## Tests ##
add_executable(json_parse_test
    test/json_parse_test.cpp
)

target_include_directories(json_parse_test PUBLIC
    ${WD_INCLUDE_DIR}
)

target_compile_features(json_parse_test PRIVATE
    cxx_std_17
)

target_link_libraries(json_parse_test PRIVATE
    WeatherData
    GTest::gtest_main
)

