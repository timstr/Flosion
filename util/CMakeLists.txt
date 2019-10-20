cmake_minimum_required(VERSION 3.10.0)

set(include_path "include/Flosion/Util")

set(flosion_util_headers
	${include_path}/FFT.hpp
	${include_path}/FileBrowser.hpp
	${include_path}/Pi.hpp
    ${include_path}/RNG.hpp
)

set(flosion_util_srcs
    src/RNG.cpp
)

add_library(flosion_util STATIC ${flosion_util_headers} ${flosion_util_srcs})

set_property(TARGET flosion_util PROPERTY CXX_STANDARD 17)

target_include_directories(flosion_util
	PUBLIC include
)
