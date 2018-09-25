cmake_minimum_required(VERSION 3.10.0)

set(audio-srcs
	audio.h
	audioUI.h
)

add_library(audio STATIC ${audio-srcs})

set_target_properties(audio PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET audio PROPERTY CXX_STANDARD 17)

target_link_libraries(audio PUBLIC flosion-objects-base)