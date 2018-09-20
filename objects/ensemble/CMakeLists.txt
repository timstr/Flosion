cmake_minimum_required(VERSION 3.10.0)

set(ensemble-srcs
	ensemble.h
	ensembleUI.h
)

add_library(ensemble STATIC ${ensemble-srcs})

set_target_properties(ensemble PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(ensemble PUBLIC flosion-objects-base)