cmake_minimum_required(VERSION 3.10.0)

set(functions-srcs
	functions.h
	functionsUI.h
)

add_library(functions STATIC ${functions-srcs})

set_target_properties(functions PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET functions PROPERTY CXX_STANDARD 17)

target_link_libraries(functions PUBLIC flosion-objects-base)