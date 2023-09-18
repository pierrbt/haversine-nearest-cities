/*
 * Haversine Calculator
 * Made by Pierre BIDET @pierrbt
 * CC BY-NC-SA
 * 2023-09-18
 */



#include <iostream>
#include "haversine.hpp"
#include <fstream>
#include <utility>
#include <vector>
#include <map>

#define REGION "FR"

struct City {
    City(std::string name, double lat, double lon) {
        this->name = std::move(name);
        this->lat = lat;
        this->lon = lon;
    }

    std::string name;
    double lat;
    double lon;
};

size_t getCityIndex(const std::vector<struct City> &extracted_cities, const std::string &question) {
    size_t index = -1;
    for (int i = 0; i < extracted_cities.size(); ++i) {
        if (extracted_cities[i].name == question) {
            index = i;
            break;
        }
    }
    return index;
}

std::vector<struct City> getExtractedCities() {

    auto file = std::ifstream("../cities.csv", std::ios::in);

    if(!file.is_open())
    {
        std::cerr << "File not found";
        exit(1);
    }


    std::vector<struct City> extracted_cities;
    std::string line;

    while (std::getline(file, line)) {
        std::vector<std::string> line_buffer;
        std::string char_buffer;
        for (const auto &c: line) {
            if (c == '\n') {
                line_buffer.push_back(char_buffer);
                char_buffer.clear();
                break;
            } else if (c == ',') {
                line_buffer.push_back(char_buffer);
                char_buffer.clear();
            } else if (c == '"') {
                continue;
            } else {
                char_buffer += c;
            }
        }
        if (line_buffer.size() == 10 && line_buffer[6] == REGION) {
            extracted_cities.emplace_back(
                    line_buffer[1],
                    std::stod(line_buffer[8]),
                    std::stod(line_buffer[9]));
        }
    }
    return extracted_cities;
}

size_t askCity(const std::vector<struct City> &extracted_cities) {
    std::string input;
    int index = -1;

    while (index < 0) {
        std::cout << "Entrez la ville dont vous voulez connaitre les proximites : ";
        std::cin >> input;
        index = getCityIndex(extracted_cities, input);
        std::cout << "\n";
    }
    return index;
}

std::map<std::string, double> getNearCities(const std::vector<struct City> &extracted_cities, int index) {
    std::map<std::string, double> distances;
    for (int i = 0; i < extracted_cities.size(); ++i) {

        static double lat = extracted_cities[index].lat;
        static double lon = extracted_cities[index].lon;
        if (i == index)
            continue;
        const City &city = extracted_cities[i];
        distances[extracted_cities[i].name] = calculate_distance(
                lat,
                lon,
                city.lat,
                city.lon
        );
    }
    return distances;
}

int main() {
    std::vector<struct City> extracted_cities = getExtractedCities();

    int index = askCity(extracted_cities);

    std::map<std::string, double> distances = getNearCities(extracted_cities, index);

    for (const auto &distance: distances) {
        if (distance.second > 20)
            continue;

        static std::string name = extracted_cities[index].name;
        std::cout << name << " - " << distance.first << " : " << distance.second << "km\n";
    }

    return 0;
}