cmake_minimum_required(VERSION 3.9.0)

set(filter-srcs
	filter.h
	filterUI.h
)

add_library(filter STATIC ${filter-srcs})

set_target_properties(filter PROPERTIES LINKER_LANGUAGE CXX)