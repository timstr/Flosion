cmake_minimum_required(VERSION 3.10.0)

set(flosion-ui-headers
	BoxUI.h
	FlosionUI.h
	Font.h
	NumberWireUI.h
	ObjectUI.h
	ObjectFactory.h
	SoundWireUI.h
	WireBase.h
)

set(flosion-ui-srcs
	BoxUI.cpp
	FlosionUI.cpp
	Font.cpp
	NumberWireUI.cpp
	ObjectUI.cpp
	SoundWireUI.cpp
	WireBase.cpp
)

add_library(flosion-ui STATIC ${flosion-ui-headers} ${flosion-ui-srcs})

set_property(TARGET flosion-ui PROPERTY CXX_STANDARD 17)

target_link_libraries(flosion-ui
	PUBLIC tims-gui
	PUBLIC flosion-core
	PUBLIC flosion-objects
)

target_include_directories(flosion-ui PUBLIC ".")