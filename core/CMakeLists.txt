cmake_minimum_required(VERSION 3.10.0)

set(flosion-core-headers
	musical.h
	MultiInput.h
	NumberInput.h
	NumberSource.h
	SingleInput.h
	SoundInput.h
	SoundQueue.h
	SoundResult.h
	SoundSource.h
	Stateful.h
)

set(flosion-core-srcs
	NumberInput.cpp
	NumberSource.cpp
	SingleInput.cpp
	SoundInput.cpp
	SoundQueue.cpp
	SoundResult.cpp
	SoundSource.cpp
	Stateful.cpp
)

add_library(flosion-core STATIC ${flosion-core-headers} ${flosion-core-srcs})

set_target_properties(flosion-core PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET flosion-core PROPERTY CXX_STANDARD 17)

target_include_directories(flosion-core
	PUBLIC "."
	PUBLIC ${flosion-util_SOURCE_DIR}
)

target_link_libraries(flosion-core flosion-util)