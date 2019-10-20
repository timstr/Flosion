cmake_minimum_required(VERSION 3.10.0)

set(include_path "include/Flosion/UI/Objects")

set(flosion_ui_objects_headers
    ${include_path}/Functions.hpp
)

set(flosion_ui_objects_srcs
    src/Functions.cpp
)

add_library(flosion_ui_objects STATIC ${flosion_ui_objects_headers} ${flosion_ui_objects_srcs})

set_property(TARGET flosion_ui_objects PROPERTY CXX_STANDARD 17)

target_link_libraries(flosion_ui_objects
    PUBLIC flosion_ui_core
    PUBLIC flosion_objects
)

target_include_directories(flosion_ui_objects PUBLIC include)
