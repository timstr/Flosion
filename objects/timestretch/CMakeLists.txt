cmake_minimum_required(VERSION 3.10.0)

set(timestretch-srcs
	timestretch.h
	timestretchUI.h
)

add_library(timestretch STATIC ${timestretch-srcs})

set_target_properties(timestretch PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET timestretch PROPERTY CXX_STANDARD 17)

target_link_libraries(timestretch PUBLIC flosion-objects-base)