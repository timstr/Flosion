cmake_minimum_required(VERSION 3.9.0)

set(flosion-core-headers
	musical.h
	NumberInput.h
	NumberResult.h
	NumberSource.h
	SoundQueue.h
	SoundResult.h
	SoundSource.h
	Stateful.h
)

set(flosion-core-srcs
	musical.h
	NumberInput.h
	NumberResult.h
	NumberSource.h
	SoundQueue.h
	SoundResult.h
	SoundSource.h
	Stateful.h
)

add_library(flosion-core STATIC ${flosion-core-headers} ${flosion-core-srcs})

set_target_properties(flosion-core PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(flosion-core
	PUBLIC "."
	PUBLIC ${flosion-util_SOURCE_DIR}
)

target_link_libraries(flosion-core flosion-util)