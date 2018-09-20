cmake_minimum_required(VERSION 3.10.0)

project(flosion)

set(flosion-util-headers
	fft.h
	filebrowser.h
	hannwindow.h
	pi.h
	sstreamutil.h
)

set(flosion-util-srcs
	hannwindow.cpp
	stringutil.cpp
)

add_library(flosion-util STATIC ${flosion-util-headers} ${flosion-util-srcs})

target_include_directories(flosion-util
	PUBLIC "."
	PUBLIC ${flosion-core_SOURCE_DIR}
)

target_link_libraries(flosion-util flosion-core)