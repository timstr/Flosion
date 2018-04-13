cmake_minimum_required(VERSION 3.9.0)

set(spline-srcs
	spline.h
	splineUI.h
)

add_library(spline STATIC ${spline-srcs})

set_target_properties(spline PROPERTIES LINKER_LANGUAGE CXX)