cmake_minimum_required(VERSION 3.21)

project(aoc2024)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(Catch2)

include_directories(src)

add_executable(run_tests test/tests.cpp)
target_link_libraries(run_tests PRIVATE Catch2::Catch2WithMain)

add_executable(day1 src/day1.cpp)
add_executable(day2 src/day2.cpp)
add_executable(day3 src/day3.cpp)
