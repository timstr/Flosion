cmake_minimum_required(VERSION 3.9.0)

set(wavegen-headers
	wavegen.h
	wavegenUI.h
)

add_library(wavegen STATIC ${wavegen-headers})

set_target_properties(wavegen PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(wavegen PUBLIC ".")