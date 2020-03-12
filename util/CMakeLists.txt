cmake_minimum_required(VERSION 3.10.0)

set(include_path "include/Flosion/Util")

set(flosion_util_headers
	${include_path}/Base64.hpp
	${include_path}/FFT.hpp
	${include_path}/FileBrowser.hpp
	${include_path}/Pi.hpp
    ${include_path}/RNG.hpp
    ${include_path}/Volume.hpp
)

set(flosion_util_srcs
    src/Base64.cpp
    src/FileBrowser.cpp
    src/RNG.cpp
    src/Volume.cpp
)

add_library(flosion_util STATIC ${flosion_util_headers} ${flosion_util_srcs})

set_property(TARGET flosion_util PROPERTY CXX_STANDARD 17)

target_include_directories(flosion_util
	PUBLIC include
)
