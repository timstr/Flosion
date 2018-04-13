cmake_minimum_required(VERSION 3.9.0)

set(audio-srcs
	audio.h
	audioUI.h
)

add_library(audio STATIC ${audio-srcs})

set_target_properties(audio PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(audio PUBLIC flosion-objects-base)