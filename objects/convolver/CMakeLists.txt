cmake_minimum_required(VERSION 3.9.0)

set(convolver-headers
	convolver.h
	convolverUI.h
)

add_library(convolver STATIC ${convolver-headers})

set_target_properties(convolver PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(convolver PUBLIC ".")