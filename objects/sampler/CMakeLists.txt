cmake_minimum_required(VERSION 3.9.0)

set(sampler-srcs
	sampler.h
	samplerUI.h
)

add_library(sampler STATIC ${sampler-srcs})

set_target_properties(sampler PROPERTIES LINKER_LANGUAGE CXX)