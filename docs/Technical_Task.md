# Technical Task (C++ Developer)

Thank you for your interest in the C++ Developer position. The following exercise is meant to test your ability to:
1. Translate requirements to code
2. Write and document C++ code using object oriented programming
3. Handle edge cases and error conditions
4. Write unit tests
5. Problem solve

## Challenge

At the bottom of this file there is an example weather data payload.
For this challenge, you should write a C++ script that takes in this weather data and parses it according to these three command line arguments (the weather data itself can be passed in as an additional argument to the following or read from a file, either is acceptable):
* -d or --date
  * Inputs: a string in “YYYY-MM-DD” format
  * Behavior: argument represents a specific day to retrieve weather data for
* -r or --range
  * Inputs: a string in “YYYY-MM-DD|YYYY-MM-DD” format
  * Behavior: return all weather data from the specific time range in the same format as the input payload
* -m or –mean
  * Inputs: a string in “YYYY-MM-DD|YYYY-MM-DD” format and one string representing any of “tmax”, “tmin”, “tmin”, “ppt”
  * Behavior: return the mean of the variable provided over the specific time range

### Bonus

Time allowing an extra challenge is completing the following:

* -h or --historical-sample
  * Inputs: a string in “YYYY-MM-DD|YYYY-MM-DD” format and a string in “YYYY|YYYY” format
  * Behavior: return all weather data as in “-r” but instead of simply fetching the data, for a given day randomly sample from the years in the second arg. For example, if someone supplied “2022-01-01|2022-12-31” “2018|2022” weather data from for all of 2022 should be returned but the source for any given day should be that day from any year from 2018 to 2022. (For clarity, the years in the expected response in this example should be 2022)
