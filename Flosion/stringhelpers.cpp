#include "stringhelpers.h"
#include <sstream>
#include <limits>

namespace {
	std::stringstream stream;
}

std::string toString(float x){
	stream.str("");
	stream.clear();
	stream << x;
	return stream.str();
}

std::string toString(double x){
	stream.str("");
	stream.clear();
	stream << x;
	return stream.str();
}

std::string toString(int x){
	stream.str("");
	stream.clear();
	stream << x;
	return stream.str();
}

float stringToFloat(const std::string& str){
	stream.str(str);
	stream.clear();
	float x;
	stream >> x;
	if (!stream.fail() && stream.eof()){
		return x;
	} else {
		return std::numeric_limits<float>::quiet_NaN();
	}
}

double stringToDouble(const std::string& str){
	stream.str(str);
	stream.clear();
	double x;
	stream >> x;
	if (!stream.fail()){
		return x;
	} else {
		return std::numeric_limits<double>::quiet_NaN();
	}
}

int stringToInt(const std::string& str, int default_value = 0){
	stream.str(str);
	stream.clear();
	int x;
	stream >> x;
	if (!stream.fail()){
		return x;
	} else {
		return default_value;
	}
}