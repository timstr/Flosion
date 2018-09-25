cmake_minimum_required(VERSION 3.10.0)

set(distortion-srcs
	distortion.h
	distortionUI.h
)

add_library(distortion STATIC ${distortion-srcs})

set_target_properties(distortion PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET distortion PROPERTY CXX_STANDARD 17)

target_link_libraries(distortion PUBLIC flosion-objects-base)