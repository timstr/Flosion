cmake_minimum_required(VERSION 3.9.0)

set(functions-srcs
	functions.h
	functionsUI.h
)

add_library(functions STATIC ${functions-srcs})

set_target_properties(functions PROPERTIES LINKER_LANGUAGE CXX)