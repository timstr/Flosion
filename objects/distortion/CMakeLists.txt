cmake_minimum_required(VERSION 3.9.0)

set(distortion-headers
	distortion.h
	distortionUI.h
)

add_library(distortion STATIC ${distortion-headers})

set_target_properties(distortion PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(distortion PUBLIC ".")