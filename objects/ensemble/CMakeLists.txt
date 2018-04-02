cmake_minimum_required(VERSION 3.9.0)

set(ensemble-headers
	ensemble.h
	ensembleUI.h
)

add_library(ensemble STATIC ${ensemble-headers})

set_target_properties(ensemble PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(ensemble PUBLIC ".")