cmake_minimum_required(VERSION 3.9.0)

set(wavegen-srcs
	wavegen.h
	wavegenUI.h
)

add_library(wavegen STATIC ${wavegen-srcs})

set_target_properties(wavegen PROPERTIES LINKER_LANGUAGE CXX)