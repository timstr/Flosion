cmake_minimum_required(VERSION 3.9.0)

set(wavetable-srcs
	wavetable.h
	wavetableUI.h
)

add_library(wavetable STATIC ${wavetable-srcs})

set_target_properties(wavetable PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(wavetable PUBLIC flosion-objects-base)