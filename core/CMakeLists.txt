cmake_minimum_required(VERSION 3.10.0)

set(include_path "include/Flosion/Core")

set(flosion_core_headers
	${include_path}/BorrowingNumberSource.hpp
	${include_path}/BorrowingNumberSource.tpp
	${include_path}/Immovable.hpp
	${include_path}/MultiSoundInput.hpp
	${include_path}/MultiSoundInput.tpp
	${include_path}/Network.hpp
	${include_path}/NumberNode.hpp
	${include_path}/NumberResult.hpp
	${include_path}/NumberSource.hpp
	${include_path}/RecursiveSharedMutex.hpp
	${include_path}/Sample.hpp
	${include_path}/SingleSoundInput.hpp
	${include_path}/SoundChunk.hpp
	${include_path}/SoundInput.hpp
	${include_path}/SoundNode.hpp
	${include_path}/SoundNode.tpp
	${include_path}/SoundResult.hpp
	${include_path}/SoundSource.hpp
	${include_path}/SoundSource.tpp
	${include_path}/SoundSourceTemplate.hpp
	${include_path}/SoundSourceTemplate.tpp
	${include_path}/SoundState.hpp
	${include_path}/SoundState.tpp
	${include_path}/State.hpp
	${include_path}/StateAllocator.hpp
	${include_path}/StateTable.hpp
	${include_path}/StateBorrower.hpp
	${include_path}/StatefulNumberSource.hpp
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

set_property(TARGET flosion_core PROPERTY CXX_STANDARD 17)

set_target_properties(flosion_core PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET flosion_core PROPERTY CXX_STANDARD 17)

target_include_directories(flosion_core
	PUBLIC include
)

target_link_libraries(flosion_core
    PUBLIC flosion_util
)