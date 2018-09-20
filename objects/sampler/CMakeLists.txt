cmake_minimum_required(VERSION 3.10.0)

set(sampler-srcs
	sampler.h
	samplerUI.h
)

add_library(sampler STATIC ${sampler-srcs})

set_target_properties(sampler PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(sampler PUBLIC flosion-objects-base)