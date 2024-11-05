
#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

int parse(const std::string& filepath, json &jsonData);
void displayJsonData(const json &jsonData);

#endif