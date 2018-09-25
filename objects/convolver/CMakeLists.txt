cmake_minimum_required(VERSION 3.10.0)

set(convolver-srcs
	convolver.h
	convolverUI.h
)

add_library(convolver STATIC ${convolver-srcs})

set_target_properties(convolver PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET convolver PROPERTY CXX_STANDARD 17)

target_link_libraries(convolver PUBLIC flosion-objects-base)