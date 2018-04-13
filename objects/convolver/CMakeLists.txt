cmake_minimum_required(VERSION 3.9.0)

set(convolver-srcs
	convolver.h
	convolverUI.h
)

add_library(convolver STATIC ${convolver-srcs})

set_target_properties(convolver PROPERTIES LINKER_LANGUAGE CXX)