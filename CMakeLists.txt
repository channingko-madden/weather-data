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
    ${WD_SOURCE_DIR}/weather_data/payload_parser.cpp
)

add_library(WeatherData SHARED ${LIB_SOURCES})
target_include_directories(WeatherData PUBLIC
    ${WD_INCLUDE_DIR}
)
target_link_libraries(WeatherData
    jsoncpp
    date::date
)

## parseweather script ##
add_executable(parseweather
    src/parse_weather.cpp
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
    jsoncpp
)

#install(TARGETS appmap-app
#    BUNDLE DESTINATION .
##    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

## Tests ##
add_executable(payload_parser_test
    test/payload_parser_test.cpp
)

target_include_directories(payload_parser_test PUBLIC
    ${WD_INCLUDE_DIR}
)

target_compile_features(payload_parser_test PRIVATE
    cxx_std_17
)

target_link_libraries(payload_parser_test PRIVATE
    WeatherData
    GTest::gtest_main
)

