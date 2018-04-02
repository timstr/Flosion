cmake_minimum_required(VERSION 3.9.0)

set(filter-headers
	filter.h
	filterUI.h
)

add_library(filter STATIC ${filter-headers})

set_target_properties(filter PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(filter PUBLIC ".")