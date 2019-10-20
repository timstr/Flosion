cmake_minimum_required(VERSION 3.10.0)

set(include_path "include/Flosion/Objects")

set(flosion_objects_headers
	${include_path}/DAC.hpp
	${include_path}/Functions.hpp
	${include_path}/FunctionsBase.hpp
	${include_path}/WaveForms.hpp
	${include_path}/WaveGenerator.hpp
)

set(flosion_objects_srcs
	src/DAC.cpp
	src/Functions.cpp
	src/FunctionsBase.cpp
	src/WaveForms.cpp
	src/WaveGenerator.cpp
)

add_library(flosion_objects STATIC ${flosion_objects_headers} ${flosion_objects_srcs})

target_include_directories(flosion_objects PUBLIC include)

set_target_properties(flosion_objects PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET flosion_objects PROPERTY CXX_STANDARD 17)

target_link_libraries(flosion_objects
    PUBLIC flosion_core
    PUBLIC tims-gui
)
