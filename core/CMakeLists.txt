cmake_minimum_required(VERSION 3.10.0)

set(flosion_core_headers
	include/BorrowingNumberSource.hpp
	include/BorrowingNumberSource.tpp
	include/Immovable.hpp
	include/MultiSoundInput.hpp
	include/MultiSoundInput.tpp
	include/Network.hpp
	include/NumberNode.hpp
	include/NumberResult.hpp
	include/NumberSource.hpp
	include/RecursiveSharedMutex.hpp
	include/Sample.hpp
	include/SingleSoundInput.hpp
	include/SoundChunk.hpp
	include/SoundInput.hpp
	include/SoundNode.hpp
	include/SoundNode.tpp
	include/SoundResult.hpp
	include/SoundSource.hpp
	include/SoundSource.tpp
	include/SoundSourceTemplate.hpp
	include/SoundSourceTemplate.tpp
	include/SoundState.hpp
	include/SoundState.tpp
	include/State.hpp
	include/StateAllocator.hpp
	include/StateTable.hpp
	include/StateBorrower.hpp
	include/StatefulNumberSource.hpp
)

set(flosion_core_srcs
	src/BorrowingNumberSource.cpp
	src/Network.cpp
	src/NumberNode.cpp
	src/NumberResult.cpp
	src/NumberSource.cpp
	src/RecursiveSharedMutex.cpp
	src/Sample.cpp
	src/SingleSoundInput.cpp
	src/SoundChunk.cpp
	src/SoundInput.cpp
	src/SoundNode.cpp
	src/SoundResult.cpp
	src/SoundSource.cpp
	src/SoundState.cpp
	src/StateBorrower.cpp
	src/StateTable.cpp
)

add_library(flosion_core STATIC ${flosion_core_headers} ${flosion_core_srcs})

set_target_properties(flosion_core PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET flosion_core PROPERTY CXX_STANDARD 17)

target_include_directories(flosion_core
	PUBLIC include
)
