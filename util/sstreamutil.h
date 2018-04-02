#pragma once

#include <string>
#include <sstream>

template<typename T>
bool streamRead(std::stringstream& stream, T& dest){
	std::string contents = stream.str();
	stream >> dest;
	if (stream){
		return true;
	} else {
		stream.clear();
		stream.str(contents);
		return false;
	}
}