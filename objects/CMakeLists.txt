cmake_minimum_required(VERSION 3.9.0)

add_library(flosion-objects INTERFACE)

# list of all sub-libraries
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

# of the above sub-libraries, those that aren't header-only and thus need to be linked
set(flosion-objects-link-libraries
	dac
)


foreach(objlib ${flosion-objects-libraries})
	add_subdirectory(${objlib})
	target_include_directories(flosion-objects INTERFACE ${objlib})
endforeach(objlib)

foreach(objlib ${flosion-objects-link-libraries})
	target_link_libraries(flosion-objects INTERFACE ${objlib})
endforeach(objlib)

target_include_directories(flosion-objects
	INTERFACE ${flosion-ui_SOURCE_DIR}
	INTERFACE ${flosion-core_SOURCE_DIR}
)

target_link_libraries(flosion-objects
	INTERFACE flosion-ui
	INTERFACE flosion-core
)