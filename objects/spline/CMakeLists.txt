cmake_minimum_required(VERSION 3.10.0)

set(spline-srcs
	spline.h
	splineUI.h
)

add_library(spline STATIC ${spline-srcs})

set_target_properties(spline PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET spline PROPERTY CXX_STANDARD 17)

target_link_libraries(spline PUBLIC flosion-objects-base)