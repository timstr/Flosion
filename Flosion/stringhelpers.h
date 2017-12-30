#pragma once

#include <string>

std::string toString(float x);

std::string toString(double x);

std::string toString(int x);

float stringToFloat(const std::string& str);

double stringToDouble(const std::string& str);

int stringToInt(const std::string& str, int default_value);