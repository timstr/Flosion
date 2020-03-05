cmake_minimum_required(VERSION 3.10.0)

set(include_path "include/Flosion/UI/Core")

set(flosion_ui_core_headers
    ${include_path}/ArgumentParser.hpp
    ${include_path}/BorrowingNumberObject.hpp
    ${include_path}/Panel.hpp
    ${include_path}/PanelContextMenu.hpp
    ${include_path}/FlosionUI.hpp
    ${include_path}/Font.hpp
    ${include_path}/NumberObject.hpp
    ${include_path}/NumberPegs.hpp
    ${include_path}/NumberWire.hpp
    ${include_path}/Object.hpp
    ${include_path}/ObjectFactory.hpp
    ${include_path}/Serialization.hpp
    ${include_path}/SoundObject.hpp
    ${include_path}/SoundPegs.hpp
    ${include_path}/SoundWire.hpp
    ${include_path}/WireBase.hpp
)

set(flosion_ui_core_srcs
    src/ArgumentParser.cpp
    src/BorrowingNumberObject.cpp
    src/Panel.cpp
    src/PanelContextMenu.cpp
    src/FlosionUI.cpp
    src/Font.cpp
    src/NumberObject.cpp
    src/NumberPegs.cpp
    src/NumberWire.cpp
    src/Object.cpp
    src/ObjectFactory.cpp
    src/Serialization.cpp
    src/SoundObject.cpp
    src/SoundPegs.cpp
    src/SoundWire.cpp
    src/WireBase.cpp
)

add_library(flosion_ui_core STATIC ${flosion_ui_core_headers} ${flosion_ui_core_srcs})

set_property(TARGET flosion_ui_core PROPERTY CXX_STANDARD 17)

target_link_libraries(flosion_ui_core
    PUBLIC flosion_core
    PUBLIC tims-gui
)

target_include_directories(flosion_ui_core PUBLIC include)
