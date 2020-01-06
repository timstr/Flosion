cmake_minimum_required(VERSION 3.10.0)

project(flosion C CXX)

add_executable(flosion Flosion.cpp)

set_property(TARGET flosion PROPERTY CXX_STANDARD 17)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT flosion)

if(MSVC)
	# increase warning level
	add_definitions(/W4)

	# turn warning C4715 ("not all control paths return a value") into an error
	add_definitions(/we4715)

	# disable warning C4250 ("class X inherits virtual member function Y via dominance")
	add_definitions(/wd4250)

	# turn warning C4239 (non-standard extension that allows temporaries to be bound to
	# non-const references, yay microsoft) into an error
	# This extension is without question the source of many bugs and disastrous surprises
	add_definitions(/we4239)
else()
	SET(CMAKE_CXX_FLAGS -pthread)
endif()

add_subdirectory(tims-gui)
add_subdirectory(core)
add_subdirectory(objects)
add_subdirectory(ui)
add_subdirectory(util)
add_subdirectory(tests)

if(UNIX)
    target_link_libraries(flosion
        PUBLIC
        tims-gui
        flosion_core
        flosion_objects
        -Wl,--whole-archive
        flosion_ui_core
        flosion_ui_objects
        -Wl,--no-whole-archive
        flosion_util
    )
else()
    target_link_libraries(flosion
        PUBLIC
        tims-gui
        flosion_core
        flosion_objects
        flosion_ui_core
        flosion_ui_objects
        flosion_util
    )
endif()

# Ensure that symbols from ui libraries are always included.
# This is needed because these symbols include objects that need
# to run code before main(), but compilers like to exclude them.
if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /WHOLEARCHIVE:flosion_ui_core /WHOLEARCHIVE:flosion_ui_objects")
endif()

# copy fonts to current build directory
add_custom_command(TARGET flosion POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/ui/core/fonts"
		"$<TARGET_FILE_DIR:flosion>/fonts"
)

# copy fonts to binary directory for debugger
add_custom_command(TARGET flosion POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/ui/core/fonts"
		"${PROJECT_BINARY_DIR}/fonts"
)

# copy openal32.dll to executable directory on windows
if(WIN32)
	add_custom_command(TARGET flosion POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${SFML_DIR}/../../../bin/openal32.dll"
			"$<TARGET_FILE_DIR:flosion>"
	)
endif()
