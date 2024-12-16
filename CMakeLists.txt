cmake_minimum_required(VERSION 3.21)

project(aoc2024)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 20)

find_package(absl REQUIRED)

include_directories(src)

add_executable(day1 src/day1.cpp)
add_executable(day2 src/day2.cpp)
add_executable(day3 src/day3.cpp)
add_executable(day4 src/day4.cpp)

add_executable(day5 src/day5.cpp)
target_link_libraries(day5 absl::strings)

add_executable(day6 src/day6.cpp)

add_executable(day7 src/day7.cpp)
target_link_libraries(day7 absl::strings)
