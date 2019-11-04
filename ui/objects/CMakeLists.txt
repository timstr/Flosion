cmake_minimum_required(VERSION 3.10.0)

set(include_path "include/Flosion/UI/Objects")

set(flosion_ui_objects_headers
    ${include_path}/DAC.hpp
    ${include_path}/Ensemble.hpp
    ${include_path}/Functions.hpp
    ${include_path}/WaveForms.hpp
    ${include_path}/WaveGenerator.hpp
)

set(flosion_ui_objects_srcs
    src/DAC.cpp
    src/Ensemble.cpp
    src/Functions.cpp
    src/WaveForms.cpp
    src/WaveGenerator.cpp
)

add_library(flosion_ui_objects STATIC ${flosion_ui_objects_headers} ${flosion_ui_objects_srcs})

set_property(TARGET flosion_ui_objects PROPERTY CXX_STANDARD 17)

target_link_libraries(flosion_ui_objects
    PUBLIC flosion_ui_core
    PUBLIC flosion_objects
)

target_include_directories(flosion_ui_objects PUBLIC include)
