
#ifndef PARSER_H
#define PARSER_H

#include "common.h"

int parse(const std::string& filepath, json &jsonData);
void displayJsonData(const json &jsonData);

#endif