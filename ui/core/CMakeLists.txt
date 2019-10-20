cmake_minimum_required(VERSION 3.10.0)

set(include_path "include/Flosion/UI/Core")

set(flosion_ui_core_headers
    ${include_path}/Box.hpp
    ${include_path}/BoxContextMenu.hpp
    ${include_path}/FlosionUI.hpp
    ${include_path}/Font.hpp
    ${include_path}/NumberPegs.hpp
    ${include_path}/NumberWire.hpp
    ${include_path}/Object.hpp
    ${include_path}/ObjectFactory.hpp
    ${include_path}/SoundWire.hpp
    ${include_path}/SoundPegs.hpp
    ${include_path}/WireBase.hpp
)

set(flosion_ui_core_srcs
    src/Box.cpp
    src/BoxContextMenu.cpp
    src/FlosionUI.cpp
    src/Font.cpp
    src/NumberWire.cpp
    src/NumberPegs.cpp
    src/Object.cpp
    src/ObjectFactory.cpp
    src/SoundWire.cpp
    src/SoundPegs.cpp
    src/WireBase.cpp
)

add_library(flosion_ui_core STATIC ${flosion_ui_core_headers} ${flosion_ui_core_srcs})

set_property(TARGET flosion_ui_core PROPERTY CXX_STANDARD 17)

target_link_libraries(flosion_ui_core
    PUBLIC flosion_core
    PUBLIC tims-gui
)

target_include_directories(flosion_ui_core PUBLIC include)
