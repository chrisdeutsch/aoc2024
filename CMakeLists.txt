cmake_minimum_required(VERSION 3.21)

project(aoc2024)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 23)

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

add_executable(day8 src/day8.cpp)
add_executable(day9 src/day9.cpp)
add_executable(day10 src/day10.cpp)
add_executable(day11 src/day11.cpp)
add_executable(day12 src/day12.cpp)
add_executable(day13 src/day13.cpp)
add_executable(day14 src/day14.cpp)
add_executable(day15 src/day15.cpp)
add_executable(day16 src/day16.cpp)

add_executable(day17 src/day17.cpp)
target_link_libraries(day17 PRIVATE absl::strings)

