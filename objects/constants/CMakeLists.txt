cmake_minimum_required(VERSION 3.10.0)

set(constants-srcs
	constantsUI.h
)

add_library(constants STATIC ${constants-srcs})

set_target_properties(constants PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET constants PROPERTY CXX_STANDARD 17)

target_link_libraries(constants PUBLIC flosion-objects-base)