cmake_minimum_required(VERSION 3.10.0)

set(flosion_core_headers
	include/DirectStateMap.hpp
	include/Immovable.hpp
	include/KeyedStateMap.hpp
	include/MultiSoundInput.hpp
	include/NumberInput.hpp
	include/NumberResult.hpp
	include/NumberSource.hpp
	include/Sample.hpp
	include/SingleSoundInput.hpp
	include/RealtimeSoundSource.hpp
	include/SoundChunk.hpp
	include/SoundInput.hpp
	include/SoundResult.hpp
	include/SoundSource.hpp
	include/SoundSourceTemplate.hpp
	include/SoundState.hpp
	include/State.hpp
	include/StateAllocator.hpp
	include/StateArray.hpp
	include/StateBorrower.hpp
	include/StatefulNumberSource.hpp
	include/StatelessSoundSource.hpp
	include/StateOwner.hpp
	include/StateUser.hpp

#	Sample.h
#	MultiInput.h
#	NumberInput.h
#	NumberSource.h
#	SingleInput.h
#	SoundInput.h
#	SoundQueue.h
#	SoundResult.h
#	SoundSource.h
#	Stateful.h
)

set(flosion_core_srcs
	src/NumberInput.cpp
	src/NumberResult.cpp
	src/NumberSource.cpp
	src/Sample.cpp
	src/SingleSoundInput.cpp
	src/SoundChunk.cpp
	src/SoundInput.cpp
	src/SoundResult.cpp
	src/SoundSource.cpp
	src/SoundState.cpp
	src/StateAllocator.cpp
	src/StateArray.cpp
	src/StateBorrower.cpp
	src/StateOwner.cpp
	src/StateUser.cpp

#	NumberInput.cpp
#	NumberSource.cpp
#	SingleInput.cpp
#	SoundInput.cpp
#	SoundQueue.cpp
#	SoundResult.cpp
#	SoundSource.cpp
#	Stateful.cpp
)

add_library(flosion_core STATIC ${flosion_core_headers} ${flosion_core_srcs})

set_target_properties(flosion_core PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET flosion_core PROPERTY CXX_STANDARD 17)

target_include_directories(flosion_core
	PUBLIC include
#	PUBLIC "."
#	PUBLIC ${flosion-util_SOURCE_DIR}
)
