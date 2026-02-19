#ifndef WALL_PARSER_HPP
#define WALL_PARSER_HPP

#include <vector>
#include <string>
#include <utility>

std::vector<int> parseXRange(const std::string& s);
std::vector<std::pair<int, int>> loadWalls(int width, int height);

#endif // WALL_PARSER_HPP