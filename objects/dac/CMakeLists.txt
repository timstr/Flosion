cmake_minimum_required(VERSION 3.9.0)

set(dac-headers
	dac.h
	dacUI.h
)

set(dac-srcs
	dac.cpp
)

add_library(dac ${dac-headers} ${dac-srcs})

set_target_properties(dac PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(dac
	PUBLIC "."
	PUBLIC ${tims-gui_SOURCE_DIR}
	PUBLIC ${flosion-core_SOURCE_DIR}
	
)

target_link_libraries(dac
	PUBLIC tims-gui
	PUBLIC flosion-core
)