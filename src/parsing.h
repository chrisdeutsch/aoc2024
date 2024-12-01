#ifndef PARSING_H_
#define PARSING_H_

#include <iostream>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <vector>

template <typename T, typename U> std::vector<std::tuple<T, U>> parse_input(std::istream &is) {
    std::vector<std::tuple<T, U>> result;

    std::string line;
    while (std::getline(is, line)) {
        std::istringstream ss(line);
        T key;
        U value;

        if (!(ss >> key)) {
            throw std::runtime_error("Invalid input");
        }

        if (!(ss >> value)) {
            throw std::runtime_error("Invalid input");
        };

        result.emplace_back(key, value);
    }

    return result;
}

#endif
