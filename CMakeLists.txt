cmake_minimum_required(VERSION 3.10.0)

project(flosion C CXX)

add_executable(flosion Flosion.cpp)

set_property(TARGET flosion PROPERTY CXX_STANDARD 17)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT flosion)

if(MSVC)
	add_definitions("/diagnostics:caret")
endif()

add_subdirectory(tims-gui)
add_subdirectory(core)
# add_subdirectory(ui)
# add_subdirectory(util)
# add_subdirectory(objects)
# add_subdirectory(test)

target_link_libraries(flosion
	PUBLIC
	tims-gui
	flosion_core
#	flosion-ui
#	flosion-util
#	flosion-objects
)

# copy fonts to current build directory
add_custom_command(TARGET flosion POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/ui/fonts"
		"$<TARGET_FILE_DIR:flosion>/fonts"
)

# copy fonts to binary directory for debugger
add_custom_command(TARGET flosion POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/ui/fonts"
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

if(WIN32)
	# turn warning C4715 ("not all control paths return a value") into an error
	add_definitions(/we4715)

	# disable warning C4250 ("class X inherits virtual member function Y via dominance")
	add_definitions(/wd4250)
endif()