cmake_minimum_required(VERSION 3.9.0)

set(constants-srcs
	constantsUI.h
)

add_library(constants STATIC ${constants-srcs})

set_target_properties(constants PROPERTIES LINKER_LANGUAGE CXX)