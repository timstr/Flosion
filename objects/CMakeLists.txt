cmake_minimum_required(VERSION 3.9.0)

add_library(flosion-objects INTERFACE)
add_library(flosion-objects-base INTERFACE)

target_link_libraries(flosion-objects-base
	INTERFACE flosion-core
	INTERFACE flosion-ui
	INTERFACE flosion-util
)

# list of all header-only sub-libraries
set(flosion-objects-header-libraries
	audio
	constants
	convolver
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

# list of all compiled libraries that need to be linked to
set(flosion-objects-link-libraries
	dac
)

# include every header-only library and give it access to required headers
foreach(objlib ${flosion-objects-header-libraries})
	add_subdirectory(${objlib})
	target_include_directories(flosion-objects INTERFACE ${objlib})
endforeach(objlib)

# include and link to every compiled library and give it required headers
foreach(objlib ${flosion-objects-link-libraries})
	add_subdirectory(${objlib})
	target_link_libraries(flosion-objects INTERFACE ${objlib})
	target_include_directories(flosion-objects INTERFACE ${objlib})
endforeach(objlib)