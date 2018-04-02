cmake_minimum_required(VERSION 3.9.0)

add_library(flosion-objects INTERFACE)

set(flosion-objects-libraries
	audio
	constants
	convolver
	dac
	distortion
	ensemble
	filter
	functions
	phasevocoder
	sampler
	spline
	timestretch
	wavegen
	wavetable
)

# TODO: add each of the above as a subdirectory and as a dependency

foreach(objlib ${flosion-objects-libraries})
	add_subdirectory(${objlib})
	target_link_libraries(flosion-objects INTERFACE ${objlib})
	target_include_directories(flosion-objects INTERFACE ${objlib})
endforeach(objlib)

target_include_directories(flosion-objects
	INTERFACE ${flosion-ui_SOURCE_DIR}
	INTERFACE ${flosion-core_SOURCE_DIR}
)

target_link_libraries(flosion-objects
	INTERFACE flosion-ui
	INTERFACE flosion-core
)