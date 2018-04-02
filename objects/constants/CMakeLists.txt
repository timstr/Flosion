cmake_minimum_required(VERSION 3.9.0)

set(constants-headers
	constantUI.h
)

add_library(constants STATIC ${constants-headers})

set_target_properties(constants PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(constants PUBLIC ".")