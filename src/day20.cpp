#include <iostream>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <vector>

struct position {
    std::size_t row;
    std::size_t col;
    constexpr auto operator<=>(const position &other) const = default;
};

struct racetrack {
    position start;
    position end;
    std::vector<position> track;
};

racetrack read_input(std::istream &is) {
    std::optional<position> start;
    std::optional<position> end;
    std::vector<position> track;

    std::string line;
    std::size_t row = 0;
    while (std::getline(is, line)) {
        for (const auto [col, ch] : line | std::views::enumerate) {
            if (ch == 'S') {
                start = {row, static_cast<std::size_t>(col)};
                track.emplace_back(*start);
            } else if (ch == 'E') {
                end = {row, static_cast<std::size_t>(col)};
                track.emplace_back(*end);
            } else if (ch == '.') {
                track.emplace_back(row, col);
            }
        }
        ++row;
    }

    if (!start || !end) {
        throw std::runtime_error("Error parsing racetrack");
    }

    return {*start, *end, track};
}

std::vector<position> order_track_elements(const racetrack &rt) {
    std::set<position> track_elements(rt.track.cbegin(), rt.track.cend());
    std::vector<position> track_elements_ordered;

    track_elements_ordered.push_back(rt.start);
    track_elements.erase(rt.start);

    auto pos = rt.start;
    while (pos != rt.end) {
        position new_pos;
        // North
        if (pos.row >= 1 && track_elements.contains({pos.row - 1, pos.col})) {
            new_pos = {pos.row - 1, pos.col};
        }
        // East
        if (track_elements.contains({pos.row, pos.col + 1})) {
            new_pos = {pos.row, pos.col + 1};
        }
        // South
        if (track_elements.contains({pos.row + 1, pos.col})) {
            new_pos = {pos.row + 1, pos.col};
        }
        // West
        if (pos.col >= 1 && track_elements.contains({pos.row, pos.col - 1})) {
            new_pos = {pos.row, pos.col - 1};
        }
        track_elements_ordered.push_back(new_pos);
        track_elements.erase(new_pos);
        pos = new_pos;
    }

    if (track_elements.size() > 0) {
        throw std::runtime_error("Error ordering racetrack");
    }

    return track_elements_ordered;
}

std::size_t manhatten_distance(position a, position b) {
    const auto delta_row = a.row > b.row ? a.row - b.row : b.row - a.row;
    const auto delta_col = a.col > b.col ? a.col - b.col : b.col - a.col;
    return delta_row + delta_col;
}

std::size_t get_number_of_cheats(const racetrack &rt, std::size_t cheat_len) {
    const std::size_t threshold = 100;
    std::size_t number_of_cheats = 0;

    for (const auto [start_time, start_track_element] : rt.track | std::views::enumerate) {
        for (const auto [end_time, end_track_element] :
             rt.track | std::views::enumerate | std::views::drop(start_time + 1)) {
            const auto dist = manhatten_distance(start_track_element, end_track_element);
            if (dist > cheat_len || end_time <= start_time + dist) {
                continue;
            }
            const auto time_saved = end_time - start_time - dist;
            if (time_saved >= threshold) {
                ++number_of_cheats;
            }
        }
    }
    return number_of_cheats;
}

int main() {
    auto rt = read_input(std::cin);
    rt.track = order_track_elements(rt);

    const std::size_t num_cheats_pt1 = get_number_of_cheats(rt, 2);
    std::println("Part 1: {}", num_cheats_pt1);

    const std::size_t num_cheats_pt2 = get_number_of_cheats(rt, 20);
    std::println("Part 2: {}", num_cheats_pt2);

    return 0;
}
