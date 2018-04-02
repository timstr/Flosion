cmake_minimum_required(VERSION 3.9.0)

set(spline-headers
	spline.h
	splineUI.h
)

add_library(spline STATIC ${spline-headers})

set_target_properties(spline PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(spline PUBLIC ".")