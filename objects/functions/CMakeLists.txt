cmake_minimum_required(VERSION 3.9.0)

set(functions-headers
	functions.h
	functionsUI.h
)

add_library(functions STATIC ${functions-headers})

set_target_properties(functions PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(functions PUBLIC ".")