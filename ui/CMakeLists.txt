cmake_minimum_required(VERSION 3.10.0)

set(flosion-ui-headers
	boxUI.h
	FlosionUI.h
	Font.h
	numberinputUI.h
	numberoutputUI.h
	numberwireUI.h
	objectUI.h
	soundinputUI.h
	soundoutputUI.h
	soundwireUI.h
	WireBase.h
)

set(flosion-ui-srcs
	boxUI.cpp
	FlosionUI.cpp
	Font.cpp
	numberinputUI.cpp
	numberoutputUI.cpp
	numberwireUI.cpp
	objectUI.cpp
	soundinputUI.cpp
	soundoutputUI.cpp
	soundwireUI.cpp
	WireBase.cpp
)

add_library(flosion-ui STATIC ${flosion-ui-headers} ${flosion-ui-srcs})

target_link_libraries(flosion-ui
	PUBLIC tims-gui
	PUBLIC flosion-core
	PUBLIC flosion-objects
)

target_include_directories(flosion-ui PUBLIC ".")