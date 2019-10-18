cmake_minimum_required(VERSION 3.10.0)

project(flosion)

set(flosion_util_headers
	fft.h
	filebrowser.h
	hannwindow.h
	pi.h
	sstreamutil.h
)

set(flosion_util_srcs
	hannwindow.cpp
	stringutil.cpp
)

add_library(flosion_util STATIC ${flosion_util_headers} ${flosion_util_srcs})

set_property(TARGET flosion_util PROPERTY CXX_STANDARD 17)

target_include_directories(flosion_util
	PUBLIC "."
	PUBLIC ${flosion_core_SOURCE_DIR}
)

target_link_libraries(flosion_util flosion_core)