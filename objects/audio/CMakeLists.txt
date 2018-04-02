cmake_minimum_required(VERSION 3.9.0)

set(audio-headers
	audio.h
	audioUI.h
)

add_library(audio STATIC ${audio-headers})

set_target_properties(audio PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(audio PUBLIC ".")