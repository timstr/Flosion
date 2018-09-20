cmake_minimum_required(VERSION 3.10.0)

set(phasevocoder-srcs
	phasevocoder.h
	phasevocoderUI.h
)

add_library(phasevocoder STATIC ${phasevocoder-srcs})

set_target_properties(phasevocoder PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(phasevocoder PUBLIC flosion-objects-base)