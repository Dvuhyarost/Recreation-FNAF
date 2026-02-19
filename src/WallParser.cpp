#include "WallParser.hpp"
#include <sstream>
#include <vector>
#include <string>
#include <utility>

std::vector<int> parseXRange(const std::string& s) {
    std::vector<int> result;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, ',')) {
        size_t dashPos = token.find('-');
        if (dashPos != std::string::npos) {
            int start = std::stoi(token.substr(0, dashPos));
            int end = std::stoi(token.substr(dashPos + 1));
            for (int i = start; i <= end; ++i) {
                result.push_back(i);
            }
        } else {
            result.push_back(std::stoi(token));
        }
    }
    return result;
}

std::vector<std::pair<int, int>> loadWalls(int width, int height) {
    std::vector<std::pair<int, int>> walls;
    std::vector<std::pair<int, std::string>> rawData = {
        {42, "31-80"},                       // 1
        {41, "31,44,67,80"},                 // 2
        {40, "31,44,67,80"},                 // 3
        {39, "31,80"},                       // 4
        {38, "31,80"},                       // 5 
        {37, "18-31,80"},                    // 6
        {36, "18,31,80,44,67"},              // 7
        {35, "18,31,80,44,67"},              // 8
        {34, "18,80,44-67"},                 // 9
        {33, "18,80,44,67"},                 // 10
        {32, "18,80,44,67"},                 // 11
        {31, "18,31,80,44,67"},              // 12
        {30, "18-31,44,67,80,83-101"},       // 13
        {29, "31,44,67,80,83,101"},          // 14
        {28, "31,44,67,80,83,101"},          // 15
        {27, "31,44,67,80,83,101"},          // 16
        {26, "31,44,67,80,83,101"},          // 17
        {25, "31,44,67,80,83,101"},          // 18
        {24, "20-31,44-67,80-83,91-105"},    // 19
        {23, "7-20,92,105"},                 // 20
        {22, "7,20,92,105"},                 // 21
        {21, "7,92,105"},                    // 22
        {20, "7,92,105"},                    // 23
        {19, "7,92,105"},                    // 24
        {18, "7,20,92,105"},                 // 25
        {17, "7-20,92,105"},                 // 26
        {16, "20,105"},                      // 27
        {15, "20,105"},                      // 28
        {14, "20,105"},                      // 29
        {13, "7-20,105"},                    // 30
        {12, "7,20,92,105"},                 // 31
        {11, "7,92,105"},                    // 32
        {10, "7,92,105"},                    // 33
        {9, "7,92-105"},                     // 34
        {8, "7,92"},                         // 35
        {7, "7,20,92"},                      // 36
        {6, "7-36,75-92"},                   // 37
        {5, "36,75"},                        // 38
        {4, "36,75"},                        // 39
        {3, "36,75"},                        // 40
        {2, "36,75"},                        // 41
        {1, "36-75"}                         // 42
    };

    for (const auto& [y, xs] : rawData) {
        if (y < 1 || y > height) continue;
        std::vector<int> xList = parseXRange(xs);
        for (int x : xList) {
            if (x >= 0 && x < width) {
                int screenY = y - 1;
                if (screenY < height) {
                    walls.emplace_back(x, screenY);
                }
            }
        }
    }
    return walls;
}