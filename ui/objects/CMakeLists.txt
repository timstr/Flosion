cmake_minimum_required(VERSION 3.10.0)

set(include_path "include/Flosion/UI/Objects")

set(flosion_ui_objects_headers
    ${include_path}/Functions.hpp
)

set(flosion_ui_objects_srcs
    src/Functions.cpp
)

add_library(flosion_ui_objects STATIC ${flosion_ui_objects_headers} ${flosion_ui_objects_srcs})

target_link_libraries(flosion_ui_objects
    PUBLIC flosion_ui_core
)

target_include_directories(flosion_ui_objects PUBLIC include)
