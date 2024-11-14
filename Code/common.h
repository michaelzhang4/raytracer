#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <stdexcept>
#include <string>
#include <utility>
#include <fstream>
#include "utilities.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

constexpr float EPSILON = 1e-8;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#endif // COMMON_H
