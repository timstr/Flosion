cmake_minimum_required(VERSION 3.9.0)

set(sampler-headers
	sampler.h
	samplerUI.h
)

add_library(sampler STATIC ${sampler-headers})

set_target_properties(sampler PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(sampler PUBLIC ".")