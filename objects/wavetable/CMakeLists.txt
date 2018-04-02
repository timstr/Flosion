cmake_minimum_required(VERSION 3.9.0)

set(wavetable-headers
	wavetable.h
	wavetableUI.h
)

add_library(wavetable STATIC ${wavetable-headers})

set_target_properties(wavetable PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(wavetable PUBLIC ".")