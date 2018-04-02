cmake_minimum_required(VERSION 3.9.0)

set(phasevocoder-headers
	phasevocoder.h
	phasevocoderUI.h
)

add_library(phasevocoder STATIC ${phasevocoder-headers})

set_target_properties(phasevocoder PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(phasevocoder PUBLIC ".")