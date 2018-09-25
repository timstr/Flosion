cmake_minimum_required(VERSION 3.10.0)

set(ensemble-srcs
	ensemble.h
	ensembleUI.h
)

add_library(ensemble STATIC ${ensemble-srcs})

set_target_properties(ensemble PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET ensemble PROPERTY CXX_STANDARD 17)

target_link_libraries(ensemble PUBLIC flosion-objects-base)