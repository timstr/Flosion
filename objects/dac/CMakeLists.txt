cmake_minimum_required(VERSION 3.10.0)

set(dac-headers
	DAC.h
	DACUI.h
)

set(dac-srcs
	DAC.cpp
)

add_library(dac ${dac-headers} ${dac-srcs})

set_target_properties(dac PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET dac PROPERTY CXX_STANDARD 17)

target_link_libraries(dac PUBLIC flosion-objects-base)