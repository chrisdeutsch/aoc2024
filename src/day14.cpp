#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <print>
#include <regex>
#include <string>

struct robot_state {
    std::size_t px;
    std::size_t py;
    int vx;
    int vy;
};

std::vector<robot_state> read_input(std::istream &istream) {
    std::vector<robot_state> robots;
    const std::regex pattern("^p=([+-]?\\d+),([+-]?\\d+) v=([+-]?\\d+),([+-]?\\d+)$");

    std::string line;
    std::smatch match;
    while (std::getline(istream, line)) {
        if (std::regex_match(line, match, pattern)) {
            robots.emplace_back(robot_state{.px = std::stoul(match[1]),
                                            .py = std::stoul(match[2]),
                                            .vx = std::stoi(match[3]),
                                            .vy = std::stoi(match[4])});
        }
    }
    return robots;
}

void print_robots(std::size_t width, std::size_t height, const std::vector<robot_state> robots) {
    using coordinates = std::pair<std::size_t, std::size_t>;
    std::map<coordinates, std::size_t> robot_counts;
    for (const auto &robot : robots) {
        ++robot_counts[coordinates{robot.px, robot.py}];
    }

    for (std::size_t row = 0; row < height; ++row) {
        for (std::size_t col = 0; col < width; ++col) {
            if (auto it = robot_counts.find(coordinates{col, row}); it != robot_counts.cend()) {
                std::print("{}", it->second);
            } else {
                std::print(".");
            }
        }
        std::print("\n");
    }
}

std::size_t mod(std::size_t a, std::size_t b) {
    const auto remainder = static_cast<int>(a) % static_cast<int>(b);
    if (remainder >= 0) {
        return remainder;
    }
    return remainder + b;
}

std::vector<robot_state> simulate_n(const std::vector<robot_state> &robots, std::size_t n,
                                    std::size_t width, std::size_t height) {
    std::vector<robot_state> robots_after_n_steps(robots);
    for (auto &robot : robots_after_n_steps) {
        const auto new_px = static_cast<int>(robot.px) + static_cast<int>(n) * robot.vx;
        const auto new_py = static_cast<int>(robot.py) + static_cast<int>(n) * robot.vy;
        robot.px = mod(new_px, width);
        robot.py = mod(new_py, height);
    }
    return robots_after_n_steps;
}

std::size_t safety_factor(const std::vector<robot_state> &robots, std::size_t width,
                          std::size_t height) {
    // Assumig odd width and height
    const auto midpoint = std::make_pair(width / 2, height / 2);

    const auto q0 = std::count_if(robots.cbegin(), robots.cend(), [midpoint](const auto &r) {
        return r.px > midpoint.first && r.py > midpoint.second;
    });
    const auto q1 = std::count_if(robots.cbegin(), robots.cend(), [midpoint](const auto &r) {
        return r.px > midpoint.first && r.py < midpoint.second;
    });
    const auto q2 = std::count_if(robots.cbegin(), robots.cend(), [midpoint](const auto &r) {
        return r.px < midpoint.first && r.py > midpoint.second;
    });
    const auto q3 = std::count_if(robots.cbegin(), robots.cend(), [midpoint](const auto &r) {
        return r.px < midpoint.first && r.py < midpoint.second;
    });

    return q0 * q1 * q2 * q3;
}

std::pair<float, float> robot_variance(const std::vector<robot_state> &robots) {
    const auto N = robots.size();

    float xbar = 0, ybar = 0;
    for (const auto &robot : robots) {
        xbar += static_cast<float>(robot.px) / N;
        ybar += static_cast<float>(robot.py) / N;
    }

    float varx = 0, vary = 0;
    for (const auto &robot : robots) {
        varx += std::pow(static_cast<float>(robot.px) - xbar, 2) / N;
        vary += std::pow(static_cast<float>(robot.py) - ybar, 2) / N;
    }
    return {varx, vary};
}

int main() {
    const auto width = 101uz;
    const auto height = 103uz;
    const auto robots = read_input(std::cin);

    const auto robots_after_100 = simulate_n(robots, 100, width, height);
    const auto sf = safety_factor(robots_after_100, width, height);

    std::println("Part 1: {}", sf);

    auto seconds = 0uz;
    auto robots_step = robots;
    while (seconds < 100000) {
        robots_step = simulate_n(robots_step, 1, width, height);
        ++seconds;

        const auto [varx, vary] = robot_variance(robots_step);
        if (varx < 500 && vary < 500) {
            std::println("seconds = {} varx = {} vary = {}", seconds, varx, vary);
            break;
        }
    }
    print_robots(width, height, robots_step);
    std::println("Part 2: {}", seconds);

    return 0;
}
