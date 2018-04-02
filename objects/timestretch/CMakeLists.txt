cmake_minimum_required(VERSION 3.9.0)

set(timestretch-headers
	timestretch.h
	timestretchUI.h
)

add_library(timestretch STATIC ${timestretch-headers})

set_target_properties(timestretch PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(timestretch PUBLIC ".")